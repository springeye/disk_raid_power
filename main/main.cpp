//
// main.cpp — 固件主入口（setup/loop）
//

#include "monitor/power_manager.h"
#include "drivers/key.h"

#include "lvgl.h"
#include "app_hal.h"
#include "ui/ui.h"
#include "settings.h"

#include <WiFi.h>
#include "TaskScheduler.h"
#ifdef ARDUINO
#include "log.h"
#include <Arduino.h>
#include "drivers/bq40z80.h"
#include "monitor/cell_helper.h"
#include "monitor/monitor_api.h"
#include <Wire.h>
#include "device/kk_port_device.h"
#include "i2c_utils.h"
#include "drivers/ip2366.h"
#include "drivers/sw6306_driver.h"
#include "services/ota.h"
#include <LittleFS.h>
#include "drivers/temp_sensor.h"
#include <math.h>
#include "lv_conf.h"
#ifdef ESP32
extern "C" {
#include <esp_ota_ops.h>
}

#include "esp_efuse.h"
#endif

bool runSelfTest() {
    // 在这里放自检逻辑：外设初始化、传感器、存储、联网等
    // 返回 true 表示 ok，false 表示失败
    return true;
}

void checkPendingAndValidate() {
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            mylog.println("New firmware PENDING_VERIFY -> run self test...");
            bool ok = runSelfTest();
            if (ok) {
                mylog.println("Self test OK -> mark app valid");
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
                mylog.println("Self test FAILED -> mark invalid and reboot (roll back)");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        } else {
            mylog.printf("OTA state: %d\n", (int)ota_state);
        }
    } else {
        mylog.println("esp_ota_get_state_partition FAILED");
    }
}

unsigned long previousMillis = 0;
TaskScheduler scheduler;

// 初始化串口
static void init_serial() {
#ifdef USE_HWCDC
    USBSerial.begin(115200);
    while (!USBSerial) {
    }
#else
    Serial.begin(115200);
#endif
    delay(10);
    mylog.println("setup.....");
}

// 初始化 GPIO 和按键
static void init_gpio() {
#ifdef ESP32_S3_169
    // SDA 11
    // SDC 10
    // BQSDA 17
    // BQSDC 16
#elif defined(ESP32_169)
    pinMode(PIN_POWER_CTRL, OUTPUT);
    digitalWrite(PIN_POWER_CTRL, HIGH); // 默认拉高（符合大多数硬件需求）
#endif
    init_btn();
}

// 初始化存储（LittleFS）
static void init_storage() {
    // 初始化 LittleFS，如果挂载失败则自动格式化
    if (!LittleFS.begin(true)) {
        mylog.println("LittleFS 挂载失败！");
    } else {
        mylog.println("LittleFS 挂载成功");
        // 获取文件系统信息（可选）
        size_t total = LittleFS.totalBytes();
        size_t used = LittleFS.usedBytes();
        mylog.printf("总空间: %u 字节, 已用: %u 字节\n", total, used);
    }
}

// 初始化 HAL（显示、背光、温度传感器）
static void init_hal() {
    hal_setup();
#ifdef TFT_BLK
    pinMode(TFT_BLK, OUTPUT);
    analogWrite(TFT_BLK, 255);
#endif
#ifdef ESP32_169
    init_temp();
#endif
}

// 初始化设备（DI 注入 I2C 设备）
static void init_device() {
    // DI: 构造 I2C 总线和设备，注入到 monitor_api
    static TwoWire s_wire(1); // static 保活，不被栈销毁
    KKPortDevice *dev = new KKPortDevice(&s_wire);
    monitor_api_set_device(dev);
    monitor_api_get_device()->init();
}

// 初始化 UI
static void init_ui() {
    ui_init();
    init_cells();
    init_device();
    updateUI();
    update_cells();
}

// 初始化系统服务
static void init_services() {
    setup_ota();
}

// 注册所有调度任务
static void register_tasks() {
#ifdef ESP32_169
    scheduler.addTask(auto_power_off, 2000); // 每2秒执行一次
#endif

    scheduler.addTask(
        [] {
            // sw.feedWatchdog();
            // sw.update();
        },
        5000);
    scheduler.addTask([] { updateUI(); }, 300);
    scheduler.addTask([] { hal_loop(); }, 1);
    scheduler.addTask([] { tick_btn(); }, 0); // 立即执行一次
    scheduler.addTask([] { ota_loop(); }, 1000);
}

void setup() {
    try {
        init_serial();
        init_gpio();
        init_storage();
        init_hal();
        checkPendingAndValidate();
        init_ui();
        init_services();
        register_tasks();
    } catch (...) {
        mylog.println("未知异常");
    }
}

void loop() {
    if (digitalRead(PIN_POWER_CTRL) == LOW) {
        mylog.println("检测到关机信号，开始关机流程...");
        // esp_deep_sleep_start(); // 进入深度睡眠模式，模拟关机
    } else {
        scheduler.tick(); // 非阻塞调度所有任务
    }
}
#endif /* ARDUINO */

#ifndef ARDUINO
int main(void) {
    lv_init();

    hal_setup();

    show_gui();

    hal_loop();
}

#endif /*ARDUINO*/
