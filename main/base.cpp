//
// Created by develop on 2025/9/29.
//

#include "base.h"

#include <cmath>
#include <esp32-hal-gpio.h>
#ifdef  BLE_ENABLED
#include <ESP32Control.h>
#endif

#include <log.h>
#include <monitor_api.h>

void auto_power_off() {
    float total_power = std::fabs(bq_get_power());
#ifdef BLE_ENABLED
    if (total_power < 0.7f && !ESP32Control::isClintConnected()) {
#endif
#ifndef BLE_ENABLED
        if (total_power < 0.7f) {
#endif


            digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
            mylog.println("断电");
        }
    }
    void ble() {
#ifdef BLE_ENABLED
        ESP32Control::loop();
#endif

    }
