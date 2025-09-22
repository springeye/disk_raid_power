#include "SW6306.h"
#define REG_0x23 0x23
#define REG_0x24 0x24
#define REG_0x40 0x40
SW6306::SW6306(uint8_t addr) {
    _addr = addr;
}

void SW6306::begin() {
    Wire.begin();
    disableLowPower();
    unlockI2CWrite(true);
    enableForceControlOutputPower();
    // // 配置最大输入输出功率
    writeReg(SW6306_CTRG_PISET, 100);
    writeReg(SW6306_CTRG_POSET, 100);

    // 确保允许放电
    enableDischarge();

    // 第一次喂狗
    feedWatchdog();
    _lastFeed = millis();
}
// ===== 关闭低功耗（手册要求先执行）=====
void SW6306::disableLowPower(){
    writeReg(REG_0x23, 0x01);
}

// ===== 解锁写操作 =====
void SW6306::unlockI2CWrite(bool access100to156){
    writeReg(REG_0x24, 0x20);
    delay(5);
    writeReg(REG_0x24, 0x40);
    delay(5);
    writeReg(REG_0x24, 0x80);
    delay(5);
    if(access100to156){
        writeReg(REG_0x24, 0x81);
    }
}
void SW6306::enableForceControlOutputPower(){
    uint8_t v=readReg8(REG_0x40);
    v |= (1 << 7) | (1 << 2);
    return writeReg(REG_0x40, v);
}

void SW6306::update() {
    // 周期喂狗（建议 500ms ~ 1s 一次）
    if (millis() - _lastFeed > 800) {
        feedWatchdog();
        _lastFeed = millis();
    }

    // 如果掉电，则重新打开放电
    uint8_t sys = readReg8(SW6306_STRG_SYS_STAT);
    if (!(sys & SW6306_SYS_STAT_DISCHGING)) {
        enableDischarge();
    }
}

// ---- 私有方法 ----
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
    return Wire.available() ? Wire.read() : 0xFF;
}

uint16_t SW6306::readReg16(uint8_t reg) {
    Wire.beginTransmission(_addr);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(_addr, (uint8_t)2);
    uint8_t lo = Wire.available() ? Wire.read() : 0;
    uint8_t hi = Wire.available() ? Wire.read() : 0;
    return (uint16_t)(lo | (hi << 8));
}

uint16_t SW6306::readADC(uint8_t channel) {
    writeReg(SW6306_CTRG_ADC_SET, channel);
    delay(3);
    return readReg16(SW6306_STRG_ADCL);
}

void SW6306::feedWatchdog() {
    writeReg(SW6306_CTRG_WDSET, 0x0F); // 任意非零值即可
}

void SW6306::enableDischarge() {
    // 写 0x00 到 0x21，确保放电开关开启
    writeReg(SW6306_CTRG_DISCHG_OFF, 0x00);
}

// ---- 公共 API ----
uint16_t SW6306::readVBUS() {
    return readADC(SW6306_ADC_SET_VBUS) * 8; // 8mV/LSB
}

uint16_t SW6306::readIBUS() {
    return readADC(SW6306_ADC_SET_IBUS) * 4; // 4mA/LSB
}

bool SW6306::isC1Source() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C1SRC;
}

bool SW6306::isC1Sink() {
    return readReg8(SW6306_STRG_TYPEC) & SW6306_TYPEC_C1SNK;
}
