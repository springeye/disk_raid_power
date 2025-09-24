//
// Created by develop on 2025/9/25.
//

#include "adapter.h"
// Arduino I2C 封装
extern "C" uint8_t SW6306_Arduino_I2C_Transmit(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (pdata && len) Wire.write(pdata, len);
    uint8_t err = Wire.endTransmission();
    if (pflag) *pflag = (err == 0);
    return (err == 0);
}

extern "C" uint8_t SW6306_Arduino_I2C_Receive(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag) {
    Wire.beginTransmission(addr);
    Wire.write(reg);
    uint8_t err = Wire.endTransmission(false);
    if (err) { if (pflag) *pflag = 0; return 0; }
    uint8_t got = Wire.requestFrom((int)addr, (int)len);
    for (uint8_t i = 0; i < got; i++) pdata[i] = Wire.read();
    if (pflag) *pflag = (got == len);
    return (got == len);
}