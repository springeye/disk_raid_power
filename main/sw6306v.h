#ifndef SW6306V_POWERMONITOR_H
#define SW6306V_POWERMONITOR_H

#include <Arduino.h>
#include <Wire.h>
#include <optional>

// SW6306V I2C 默认地址 (7位地址)
// #define SW6306V_ADDR        0x50

// 关键寄存器地址 (根据数据手册定义)
#define REG_CHIP_VERSION    0x01  // 芯片版本寄存器
#define REG_CHG_VOLT_STATUS 0x0E  // 充电电压状态寄存器
#define REG_FAST_CHG_IND    0x0F  // 快充协议指示寄存器
#define REG_SYS_STATUS      0x18  // 系统状态寄存器
#define REG_IN_CURRENT_MSB  0x3A  // 输入电流MSB寄存器
#define REG_IN_CURRENT_LSB  0x3B  // 输入电流LSB寄存器
#define REG_LOW_POWER_CTRL  0x23  // 低功耗控制寄存器
#define REG_WRITE_UNLOCK    0x24  // 写解锁寄存器

// 系统状态寄存器位定义
#define BIT_CHG_ENABLE      0b00100000  // bit5: 充电使能 (输入)
#define BIT_DISCHG_ENABLE   0b00010000  // bit4: 放电使能 (输出)

struct SystemStatus {
    bool isInput = false;
    bool isOutput = false;
};

struct ChargingVoltage {
    uint16_t actualVoltage = 0; // mV
    uint16_t requestedVoltage = 0; // mV
};

struct ChargingProtocol {
    String protocolName = "Unknown";
    bool isFastCharge = false;
};

struct ChargingStatus {
    SystemStatus sys{};
    ChargingVoltage volt{};
    uint16_t inputCurrent = 0; // mA
    ChargingProtocol protocol{};
};

class SW6306V_PowerMonitor {
public:
    /**
     * @brief 构造函数
     * @param address SW6306V的I2C地址 (默认0x50)
     */
    explicit SW6306V_PowerMonitor(uint8_t address = SW6306V_ADDR);

    /**
     * @brief 初始化I2C连接
     * @param sda SDA引脚 (默认为Arduino的SDA引脚)
     * @param scl SCL引脚 (默认为Arduino的SCL引脚)
     * @return true: 初始化成功, false: 初始化失败
     */
    bool begin(uint8_t sda = SDA, uint8_t scl = SCL);
    
    /**
     * @brief 解锁I2C写操作 (必须首先调用)
     * @return true: 解锁成功, false: 解锁失败
     */
    bool unlock();
    
    // 直接返回结构体
    std::optional<SystemStatus> getSystemStatus();
    std::optional<ChargingVoltage> getChargingVoltage();
    uint16_t getInputCurrent();
    std::optional<ChargingProtocol> getChargingProtocol();
    std::optional<ChargingStatus> getChargingStatus();

    /**
     * @brief 打印完整的充电状态信息到串口
     */
    void printChargingStatus();

private:
    uint8_t _address;  // I2C设备地址
    
    /**
     * @brief 读取单个寄存器
     * @param reg 寄存器地址
     * @return 寄存器值 (0xFF表示读取失败)
     */
    uint8_t readRegister(uint8_t reg) const;

    /**
     * @brief 写入单个寄存器
     * @param reg 寄存器地址
     * @param data 要写入的数据
     * @return true: 写入成功, false: 写入失败
     */
    bool writeRegister(uint8_t reg, uint8_t data) const;
};

#endif
