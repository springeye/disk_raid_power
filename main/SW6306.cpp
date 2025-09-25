#include "SW6306.h"
#define REG_0x23 0x23
#define REG_0x24 0x24
#define REG_0x40 0x40
SW6306::SW6306(uint8_t addr) {
    _addr = addr;
}

void SW6306::begin() {
    Wire.begin();
    disableLowPower();// 关闭低功耗
    unlockI2CWrite(true);//解锁寄存器写入
    enableForceControlOutputPower();//启用强制设置输出输出功率
    // 配置最大输入输出功率
    writeReg(SW6306_CTRG_PISET, 20);
    writeReg(SW6306_CTRG_POSET, 20);
    delay(100);
    writeReg(SW6306_CTRG_PISET, 100);
    writeReg(SW6306_CTRG_POSET, 100);

    uint8_t v = readReg8(REG_0x24);
    v |= (1 << 0); // 设置bit0为1
    writeReg(REG_0x24, v);
    delay(5);



    //设置放电配置指定100W
    v = readReg8(0x100);
    v |= (1 << 3);
    v = (v & ~0x07) | 0x06;
    writeReg(0x100, v);

    delay(5);
    //设置充电配置指定100W
    uint8_t v1 = readReg8(0x101);
    v1 |= (1 << 3);
    v1 = (v1 & ~0x07) | 0x07;
    writeReg(0x107, v1);
    delay(5);
    writeReg(0x1FF,0x0);
    delay(5);
    unlockI2CWrite(false);//解锁寄存器写入

    // 允许放电
    enableDischarge();
    // 喂狗(防止关闭输出)
    feedWatchdog();
}
// ===== 关闭低功耗（手册要求先执行）=====
void SW6306::disableLowPower(){
    uint8_t v = readReg8(REG_0x23);
    v |= (1 << 0); // 设置bit0为1
    writeReg(REG_0x23, v);
}

// ===== 解锁写操作 =====
void SW6306::unlockI2CWrite(bool unlock){
    if (unlock){
        // 依次将0x20, 0x40, 0x80写入24寄存器的7-5位
        uint8_t values[4] = {0x20, 0x40, 0x80,0x81};
        for (int i = 0; i < 4; ++i) {
            uint8_t v = readReg8(REG_0x24);
            v &= ~(0xE0); // 清除bit7-5
            v |= values[i]; // 设置bit7-5
            writeReg(REG_0x24, v);
            delay(5);
        }
    }else
    {
        uint8_t v = readReg8(REG_0x24);
        v &= ~(0xE0); // 清除bit7-5
        v |= 0x0; // 设置bit7-5
        writeReg(REG_0x24, v);
        delay(5);
    }

}
void SW6306::enableForceControlOutputPower(){
    uint8_t v=readReg8(REG_0x40);
    v |= (1 << 7) | (1 << 2);
    return writeReg(REG_0x40, v);
}

void SW6306::update() {
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
