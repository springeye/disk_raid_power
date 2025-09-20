#ifndef _SW6306_H_
#define _SW6306_H_

#include <Arduino.h>
#include <Wire.h>

// #define SW6306_ADDR SW6306V_ADDR  // I2C 地址（7bit）

// ---- ADC 寄存器 ----
#define SW6306_CTRG_ADC_SET 0x30
#define SW6306_STRG_ADCL   0x31

// ---- ADC 通道 ----
#define SW6306_ADC_SET_VBUS 0x00
#define SW6306_ADC_SET_IBUS 0x01

// ---- Type-C 状态寄存器 ----
#define SW6306_STRG_TYPEC   0x19
#define SW6306_TYPEC_C1SRC  0x80
#define SW6306_TYPEC_C1SNK  0x40
#define SW6306_TYPEC_C2SRC  0x20
#define SW6306_TYPEC_C2SNK  0x10

class SW6306 {
public:
    SW6306(uint8_t addr = SW6306_ADDR);

    void begin();

    uint16_t readVBUS();   // Type-C VBUS 电压 (mV)
    uint16_t readIBUS();   // Type-C IBUS 电流 (mA)

    bool isC1Source();     // C1 作为 Source (输出)
    bool isC1Sink();       // C1 作为 Sink (输入)
    bool isC2Source();     // C2 作为 Source (输出)
    bool isC2Sink();       // C2 作为 Sink (输入)

private:
    uint8_t _addr;

    void writeReg(uint8_t reg, uint8_t val);
    uint8_t readReg8(uint8_t reg);
    uint16_t readReg16(uint8_t reg);
    uint16_t readADC(uint8_t channel);
};
extern SW6306 sw;  // 默认地址 0x3C
#endif
