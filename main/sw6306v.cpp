//
// Created by develop on 2025/9/15.
//

#include "sw6306v.h"
#include <Wire.h>

SW6306V::SW6306V(uint8_t address) : _address(address) {}

bool SW6306V::begin(uint8_t sda, uint8_t scl) {
    Wire.begin(sda, scl);
    Wire.setClock(100000); // 100kHz I2C速度

    // 尝试读取芯片版本验证连接
    return getChipVersion() != 0xFF;
}

bool SW6306V::unlock() {
    // 根据数据手册第10页的解锁流程
    // Step 1: 禁用低功耗模式 (REG0x23 bit0 = 1)
    if (!writeRegister(REG_LOW_POWER_CTRL, 0x01)) return false;
    delay(5);

    // Step 2: 发送解锁序列 0x20 -> 0x40 -> 0x80
    if (!writeRegister(REG_WRITE_UNLOCK, 0x20)) return false;
    delay(1);

    if (!writeRegister(REG_WRITE_UNLOCK, 0x40)) return false;
    delay(1);

    if (!writeRegister(REG_WRITE_UNLOCK, 0x80)) return false;
    delay(5);

    return true;
}

uint8_t SW6306V::readRegister(uint8_t reg) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0) { // 保持连接以进行读取
        return 0xFF; // 传输失败
    }

    if (Wire.requestFrom(_address, (uint8_t)1) != 1) {
        return 0xFF; // 读取失败
    }
    return Wire.read();
}

bool SW6306V::writeRegister(uint8_t reg, uint8_t data) {
    Wire.beginTransmission(_address);
    Wire.write(reg);
    Wire.write(data);
    return (Wire.endTransmission() == 0);
}

uint8_t SW6306V::getChipVersion() {
    uint8_t version = readRegister(REG_CHIP_VERSION);
    return (version == 0xFF) ? 0xFF : (version & 0x0F);
}

const char* SW6306V::getActualVoltage(uint8_t status) {
    // 根据数据手册第15页表4-1
    static const char* volt_map[] = {"5V", "9V", "10V", "12V", "15V", "20V"};
    uint8_t idx = (status >> 4) & 0x07; // bits 6-4
    return (idx < 6) ? volt_map[idx] : "Unknown";
}

const char* SW6306V::getFastChargeProtocol(uint8_t status) {
    // 根据数据手册第15页表4-2
    static const char* protocol_map[] = {
        "None", "QC2", "QC3", "QC3+", "FCP", "SCP", "PD FIX", "PD PPS",
        "PE1.1", "PE2.0", "VOOC1.0", "VOOC4.0", "SuperVOOC", "SFCP", "AFC", "UFCS"
    };
    uint8_t idx = status & 0x0F; // bits 3-0
    return (idx < 16) ? protocol_map[idx] : "Unknown";
}

bool SW6306V::isCharging(uint8_t status) {
    // 根据数据手册第16页，REG_SYS_STATUS bit5
    return (status & BIT_CHG_ENABLE) != 0;
}

bool SW6306V::isDischarging(uint8_t status) {
    // 根据数据手册第16页，REG_SYS_STATUS bit4
    return (status & BIT_DISCHG_ENABLE) != 0;
}

void SW6306V::printChargingStatus() {
    uint8_t volt_status = readRegister(REG_CHG_VOLT_STATUS);
    uint8_t fastchg_ind = readRegister(REG_FAST_CHG_IND);
    uint8_t sys_status = readRegister(REG_SYS_STATUS);

    if (volt_status == 0xFF || fastchg_ind == 0xFF || sys_status == 0xFF) {
        Serial.println("Error reading SW6306V status!");
        return;
    }

    Serial.println("[SW6306V Status]");
    Serial.print("  Charging: ");
    Serial.println(isCharging(sys_status) ? "ON" : "OFF");
    Serial.print("  Discharging: ");
    Serial.println(isDischarging(sys_status) ? "ON" : "OFF");
    Serial.print("  Actual Voltage: ");
    Serial.println(getActualVoltage(volt_status));
    Serial.print("  FastCharge Protocol: ");
    Serial.println(getFastChargeProtocol(fastchg_ind));
}