#ifndef _SW6306_H_
#define _SW6306_H_

#include <Arduino.h>
#include <Wire.h>

// #define SW6306_ADDR 0x3C  // 默认 I2C 地址（7bit）

// ---- ADC ----
#define SW6306_CTRG_ADC_SET 0x30
#define SW6306_STRG_ADCL   0x31
#define SW6306_ADC_SET_VBUS 0x00
#define SW6306_ADC_SET_IBUS 0x01

// ---- 状态寄存器 ----
#define SW6306_STRG_TYPEC   0x19
#define SW6306_TYPEC_C1SRC  0x80
#define SW6306_TYPEC_C1SNK  0x40

#define SW6306_STRG_SYS_STAT 0x18
#define SW6306_SYS_STAT_CHGING    0x20
#define SW6306_SYS_STAT_DISCHGING 0x10

// ---- 控制寄存器 ----
#define SW6306_CTRG_PISET   0x45  // 输入功率限制 (W)
#define SW6306_CTRG_POSET   0x4F  // 输出功率限制 (W)
#define SW6306_CTRG_DISCHG_OFF 0x21
#define SW6306_DISCHG_OFF  0x20   // 禁止放电
#define SW6306_CTRG_WDSET  0x2B   // 喂狗

class SW6306 {
public:
    SW6306(uint8_t addr = SW6306_ADDR, TwoWire* wire = &Wire);

    void begin();       // 初始化
    void update();      // 周期调用，维持状态
    // ===== I2C 解锁相关 =====
    void disableLowPower();        // REG0x23=0x01，关闭低功耗，允许写入其它寄存器
    void unlockI2CWrite(bool unlock = false);
    void enableForceControlOutputPower();  // REG0x40 bit7 控制强制输出功率
    uint16_t readVBUS();   // C1 电压 (mV)
    uint16_t readIBUS();   // C1 电流 (mA)

    bool isC1Source();     // C1 是否作为输出
    bool isC1Sink();       // C1 是否作为输入
    void feedWatchdog();
    void enableDischarge();
private:
    uint8_t _addr;
    TwoWire* _wire;

    void writeReg(uint8_t reg, uint8_t val);
    uint8_t readReg8(uint8_t reg);
    uint16_t readReg16(uint8_t reg);
    uint16_t readADC(uint8_t channel);



};
extern SW6306 sw;  // 默认地址 0x3C
#endif
