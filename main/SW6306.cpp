#include "SW6306.h"

SW6306::SW6306(uint8_t addr) {
    _addr = addr;
}

void SW6306::begin() {
    Wire.begin();
}

void SW6306::writeReg(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t SW6306::readReg8(uint8_t reg) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)1);
    return Wire.read();
}

uint16_t SW6306::readReg16(uint8_t reg) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)2);
    uint16_t val = Wire.read() | (Wire.read() << 8);
    return val;
}

uint16_t SW6306::readADC(uint8_t channel) {
    writeReg(SW6306_CTRG_ADC_SET, channel);
    delay(2); // 等待转换
    return readReg16(SW6306_STRG_ADCL);
}

// ---- Type-C 电压电流 ----
uint16_t SW6306::readVBUS() {
    return readADC(SW6306_ADC_SET_VBUS) * 8; // 8mV/LSB
}

uint16_t SW6306::readIBUS() {
    return readADC(SW6306_ADC_SET_IBUS) * 4; // 4mA/LSB
}

// ---- Type-C 状态 ----
bool SW6306::isC1Source() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C1SRC;
}
bool SW6306::isC1Sink() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C1SNK;
}
bool SW6306::isC2Source() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C2SRC;
}
bool SW6306::isC2Sink() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C2SNK;
}
