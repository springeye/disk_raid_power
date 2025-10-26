#include <base.h>
#include <key.h>

#include "lvgl.h"
#include "app_hal.h"
#include "ui.h"
#ifdef  BLE_ENABLED
#include "ESP32Control.h"
#endif

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
        delay(10);
#ifdef  BLE_ENABLED
        ESP32Control::begin("disk_raid_power");
#endif


        mylog.println("setup.....");
        // bleManager.begin();

#ifdef ESP32_S3_169
        //SDA 11
        //SDC 10
        //BQSDA 17
        //BQSDC 16
#elifdef ESP32_169
        pinMode(12, OUTPUT);
        digitalWrite(12, HIGH); // 默认拉高（符合大多数硬件需求）
#endif
        init_btn();
        // 初始化 SPIFFS，如果挂载失败则自动格式化
        if (!SPIFFS.begin(true))
        {
            mylog.println("SPIFFS 挂载失败！");
        }else{
            mylog.println("SPIFFS 挂载成功");
            // 获取文件系统信息（可选）
            size_t total = SPIFFS.totalBytes();
            size_t used = SPIFFS.usedBytes();
            mylog.printf("总空间: %u 字节, 已用: %u 字节\n", total, used);
            // 用完后立即释放 SPIFFS 占用的 heap
            // SPIFFS.end();
        }

        // mylog.println("1111");
        hal_setup();
#ifdef TFT_BLK
        pinMode(TFT_BLK, OUTPUT);
        analogWrite(TFT_BLK, 255);
#endif
        #ifdef ESP32_169
        init_temp();
        #endif

        checkPendingAndValidate();
        // sw.begin();
        // ip2366.begin();

        ui_init();
        init_cells();
        device->init();
        updateUI();
        update_cells();


#ifdef ESP32_169
        scheduler.addTask(auto_power_off, 30*1000); // 每2秒执行一次
#endif

        // scheduler.addTask([]() {
        //     ota_loop();
        // }, 5); // 每10ms检查一次按钮状态
        scheduler.addTask([]
        {

            // sw.feedWatchdog();
            // sw.update();
        },5000);
        scheduler.addTask([]
        {
            updateUI();
        },300);
        scheduler.addTask([]
        {
            ble();
            hal_loop();
        },1);
        scheduler.addTask([]
        {
            tick_btn();
        },0); // 立即执行一次;
        scheduler.addTask([]
          {
            auto_power_off();

          },1); // 立即执行一次;

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
