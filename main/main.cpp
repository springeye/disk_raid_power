#include "lvgl.h"
#include "app_hal.h"
#include "ui.h"
#include "ESP32Control.h"
#include <WiFi.h>
#include "TaskScheduler.h"
#ifdef ARDUINO
#include <log.h>
#include <Arduino.h>
#include <bq40z80.h>
#include <cell_helper.h>
#include <monitor_api.h>
#include <i2c_utils.h>
#include <ip2366.h>
#include <SW6306.h>
#include <OneButton.h>
#include <ota.h>
#include <LittleFS.h>
#include <temp.h>
#include <math.h>
#include "lv_conf.h"
#define SPIFFS LittleFS  // 兼容旧代码
#ifdef ESP32
extern "C" {
#include <esp_ota_ops.h>
}

#include "esp_efuse.h"
#endif
#define BUTTON_PIN KEY_01
BQ40Z80 bq;
IP2366 ip2366;
SW6306 sw; // 默认地址 0x3C
// BLEManager bleManager;
OneButton btn = OneButton(
    BUTTON_PIN, // Input pin for the button
    true, // Button is active LOW
    false // Enable internal pull-up resistor
);
bool need_setup_ota = false;
char g_ip[32] = {0};
volatile bool wifi_ready = false;

void setup_ota_task(void* param)
{
    setup_ota(); // WiFi初始化
    snprintf(g_ip, sizeof(g_ip), "%u.%u.%u.%u", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2],
             WiFi.localIP()[3]);
    wifi_ready = true;
    vTaskDelete(NULL);
}

bool runSelfTest()
{
    // 在这里放自检逻辑：外设初始化、传感器、存储、联网等
    // 返回 true 表示 ok，false 表示失败
    // 为演示，这里简单返回 true
    return false;
}

void checkPendingAndValidate()
{
    const esp_partition_t* running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK)
    {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY)
        {
            mylog.println("New firmware PENDING_VERIFY -> run self test...");
            bool ok = runSelfTest();
            if (ok)
            {
                mylog.println("Self test OK -> mark app valid");
                esp_ota_mark_app_valid_cancel_rollback();
            }
            else
            {
                mylog.println("Self test FAILED -> mark invalid and reboot (roll back)");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        }
        else
        {
            mylog.printf("OTA state: %d\n", (int)ota_state);
        }
    }
    else
    {
        mylog.println("esp_ota_get_state_partition FAILED");
    }
}

unsigned long previousMillis = 0;
TaskScheduler scheduler;

void ble()
{
    ESP32Control::loop();
}

void auto_power_off()
{
    float total_power = fabs(bq_get_power());
    if (total_power < 0.7f && !ESP32Control::isClintConnected())
    {
        digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
        Serial.println("断电");
    }
}

// 定义亮度变化步长，值越大变化越快
int fadeAmount = 5;
// 存储当前亮度值，初始为0
int brightness = 0;

void setup()
{
    try
    {
#ifdef USE_HWCDC
        USBSerial.begin(115200);
        while (!USBSerial)
        {
        }
#else
        Serial.begin(115200);
#endif
        delay(1000);
        ESP32Control::begin("disk_raid_power");

        mylog.println("setup.....");
        // bleManager.begin();
        Wire.begin(26, 25);
        list_i2c_devices(Wire, 1);


        pinMode(12, OUTPUT);
        digitalWrite(12, HIGH); // 默认拉高（符合大多数硬件需求）
        //IP2366
        // pinMode(10, OUTPUT);
        // digitalWrite(10, HIGH);
        // 初始化 SPIFFS，如果挂载失败则自动格式化
        if (!SPIFFS.begin(true))
        {
            mylog.println("SPIFFS 挂载失败！");
        }
        else
        {
            mylog.println("SPIFFS 挂载成功");
            // 获取文件系统信息（可选）
            size_t total = SPIFFS.totalBytes();
            size_t used = SPIFFS.usedBytes();
            mylog.printf("总空间: %u 字节, 已用: %u 字节\n", total, used);
            // 用完后立即释放 SPIFFS 占用的 heap
            SPIFFS.end();
        }

        hal_setup();
#ifdef TFT_BLK
        pinMode(TFT_BLK, OUTPUT);
        analogWrite(TFT_BLK, 255);
#endif

        init_temp();
        checkPendingAndValidate();
        sw.begin();
        ip2366.begin();

        ui_init();
        init_cells();
        updateUI();
        update_cells();
        //
        // if (digitalRead(BUTTON_PIN) == LOW)
        // {
        //     mylog.println("btn is pressed");
        //     lv_disp_load_scr(ui_scota);
        // }
        // else
        // {
        //     mylog.println("btn not pressed");
        //     lv_disp_load_scr(ui_schome);
        // }

        // btn.attachPress([]
        // {
        //     myprintln("Single Pressed!");
        // });
        btn.attachClick([]()
        {
            mylog.println("Single click!");
        });
        btn.attachDoubleClick([]()
        {
            mylog.println("Double Pressed!");
            if (lv_scr_act() == ui_schome)
            {
                ui_scota_screen_init();
                lv_scr_load_anim(ui_scota, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
                xTaskCreate(setup_ota_task, "setup_ota", 4096, NULL, 1, NULL); // 启动异步任务
            }
            else if (lv_scr_act() == ui_scota)
            {
                ui_schome_screen_init();
                lv_scr_load_anim(ui_schome, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
                destory_ota();
            }
        });
        btn.attachLongPressStart([]()
        {
            mylog.println("Long Pressed start!");
            digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
        });
        btn.attachLongPressStop([]()
        {
            mylog.println("Long Pressed stop!");
        });
        btn.setLongPressIntervalMs(400);
        scheduler.addTask(auto_power_off, 30 * 1000); // 每2秒执行一次
        scheduler.addTask([]()
        {
            ota_loop();
        }, 5); // 每10ms检查一次按钮状态
        scheduler.addTask([]
        {
            sw.feedWatchdog();
            sw.update();
        }, 5000);
        scheduler.addTask([]
        {
            updateUI();
        }, 100);
        scheduler.addTask([]
        {
            ble();
            hal_loop();
            btn.tick();
        }, 1);
    }
    catch (...)
    {
        mylog.println("未知异常");
    }
}


void loop()
{
    scheduler.tick(); // 非阻塞调度所有任务
}
#endif /* ARDUINO */

#ifndef ARDUINO
int main(void)
{
    lv_init();

    hal_setup();

    show_gui();

    hal_loop();
}

#endif /*ARDUINO*/
