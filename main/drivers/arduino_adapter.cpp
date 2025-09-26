// Created by develop on 2025/9/26.
#include "arduino_adapter.h"
#include <Wire.h>

extern "C" {
// 异步I2C写
uint8_t ASYNC_I2C_Transmit(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    for (uint8_t i = 0; i < len; i++) {
        Wire.write(pdata[i]);
    }
    uint8_t result = Wire.endTransmission();
    if (result == 0) {
        *pflag = 1; // 传输完成
        return 1;
    } else {
        *pflag = 0; // 失败
        return 0;
    }
}

// 异步I2C读
uint8_t ASYNC_I2C_Receive(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    uint8_t result = Wire.endTransmission(false); // 保持总线
    if (result != 0) {
        *pflag = 0;
        return 0;
    }
    Wire.requestFrom(addr, len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        pdata[i] = Wire.read();
    }
    *pflag = 1;
    return 1;
}
}
