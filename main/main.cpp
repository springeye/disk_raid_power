#include "lvgl.h"
#include "app_hal.h"
#include "ui.h"

#include <WiFi.h>
#include "BLEManager.h"
#ifdef ARDUINO
#include <log.h>
#include <Arduino.h>
#include <bq40z80.h>
#include <cell_helper.h>
#include <monitor_api.h>
#include <i2c_utils.h>
#include <ip2366.h>
#include <OneButton.h>
#include <ota.h>
#include <LittleFS.h>
#include <sw6306v.h>
#include <temp.h>

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
    snprintf(g_ip, sizeof(g_ip), "%s", WiFi.localIP().toString().c_str());
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
const long interval = 10*1000; // 间隔时间(毫秒)
SW6306V_PowerMonitor powerManager;
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
        mylog.println("setup.....");
        // bleManager.begin();
        Wire.begin(26,25);
        list_i2c_devices(Wire,1);


        pinMode(12, OUTPUT);
        digitalWrite(12, HIGH); // 默认拉高（符合大多数硬件需求）
        //IP2366
        // pinMode(10, OUTPUT);
        // digitalWrite(10, HIGH);
        // 初始化 SPIFFS，如果挂载失败则自动格式化
        if (!SPIFFS.begin(true))
        {
            mylog.println("SPIFFS 挂载失败！");
        }else{
            mylog.println("SPIFFS 挂载成功");
        }

        // 获取文件系统信息（可选）
        size_t total = SPIFFS.totalBytes();
        size_t used = SPIFFS.usedBytes();
        mylog.printf("总空间: %u 字节, 已用: %u 字节\n", total, used);



        mylog.println("1111");
        hal_setup();
        mylog.println("2222");
        // lv_init();
        mylog.println("3333");
        init_temp();
        ui_init();
        init_cells();

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
        checkPendingAndValidate();
        if (!powerManager.unlock()) {
            mylog.println("Failed to unlock SW6306V!");
        }
        ip2366.begin();


    }
    catch (...)
    {
        mylog.println("未知异常");
    }
}


void loop()
{
    // bleManager.loop();
    unsigned long currentMillis = millis();
    auto total_power = bq_get_power();

    // 如果电流大于等于0.1，则不执行断电操作，重置计时器
    if (total_power >= 0.1f) {
        previousMillis = currentMillis;
    } else if (currentMillis - previousMillis >= interval) {
        //断电关机
        previousMillis = currentMillis;
        digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
        Serial.println("断电");
    }
    hal_loop();
    btn.tick();
    ota_loop();
    //TODO OTA
    // if (wifi_ready)
    // {
    //     wifi_ready = false;
    //     lv_label_set_text(ui_bat_temp, g_ip); // 显示IP到页面
    // }
    updateUI();
    update_cells();
    auto temp=read_temp();
    mylog.printf("Temperature: %.2f C\n", temp);
    if (!powerManager.unlock()) {
        mylog.println("Failed to unlock SW6306V!");
    }

    delay(300);
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
