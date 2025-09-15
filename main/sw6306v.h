//
// Created by develop on 2025/9/15.
//

#ifndef DISK_RAID_POWER_SW6306V_H
#define DISK_RAID_POWER_SW6306V_H

#include <Arduino.h>
#include <Wire.h>

// SW6306V I2C 默认地址 (7位地址)
#define SW6306V_ADDR        SW6306_ADDR

// 根据数据手册修正的关键寄存器地址
#define REG_CHIP_VERSION    0x01
#define REG_CHG_VOLT_STATUS 0x0E
#define REG_FAST_CHG_IND    0x0F
#define REG_SYS_STATUS      0x18
#define REG_NTC_CURRENT     0x1A
#define REG_LOW_POWER_CTRL  0x23
#define REG_WRITE_UNLOCK    0x24
#define REG_PAGE_SELECT     0x25  // 新增：页面选择寄存器

// 根据数据手册添加的寄存器位定义
#define BIT_CHG_ENABLE      0x20  // REG_SYS_STATUS bit5: 充电使能
#define BIT_DISCHG_ENABLE   0x10  // REG_SYS_STATUS bit4: 放电使能

class SW6306V {
public:
    // 构造函数，可选指定I2C地址
    SW6306V(uint8_t address = SW6306V_ADDR);

    // 初始化函数（可指定SDA和SCL引脚）
    bool begin(uint8_t sda = SDA, uint8_t scl = SCL);

    // 解锁I2C写操作（必须首先调用）
    bool unlock();

    // 寄存器读写函数
    uint8_t readRegister(uint8_t reg);
    bool writeRegister(uint8_t reg, uint8_t data);

    // 状态获取函数
    uint8_t getChipVersion();
    void printChargingStatus();

    // 状态解析函数
    const char* getActualVoltage(uint8_t status);
    const char* getFastChargeProtocol(uint8_t status);
    bool isCharging(uint8_t status);
    bool isDischarging(uint8_t status);

private:
    uint8_t _address;
};
#endif //DISK_RAID_POWER_SW6306V_H