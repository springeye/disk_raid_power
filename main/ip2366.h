#ifndef IP2366_H
#define IP2366_H

#include <Arduino.h>
#include <Wire.h>

class IP2366 {
public:
  // 构造函数
  IP2366(uint8_t intPin = 14);

  // 初始化函数
  void begin();

  // 读取所有数据
  void readAllData();

  // 检查是否可以通信
  bool canCommunicate();

  // 获取充电状态
  bool isCharging() const;

  // 获取充电完成状态
  bool isChargeComplete() const;

  // 获取放电状态
  bool isDischarging() const;

  // 获取Type-C输入/输出电压 (V)
  float getTypeCVoltage() const;

  // 获取Type-C输入/输出电流 (A)
  float getTypeCCurrent() const;

  // 获取系统功率 (W)
  float getSystemPower() const;

  // 获取Type-C连接状态
  bool isTypeCConnected() const;

  // 获取PD快充状态
  bool isPDCharging() const;

  // 获取原始Type-C电压 (mV)
  uint16_t getTypeCVoltageRaw() const;

  // 获取原始Type-C电流 (mA)
  uint16_t getTypeCCurrentRaw() const;

  // 获取原始系统功率 (mW)
  uint16_t getSystemPowerRaw() const;

private:
  // I2C地址
  static const uint8_t WRITE_ADDR = IP2366_ADDR;
  static const uint8_t READ_ADDR = IP2366_ADDR;

  // 寄存器地址
  static const uint8_t REG_CHARGE_STATUS = 0x31;
  static const uint8_t REG_SYS_VOLTAGE_LOW = 0x52;
  static const uint8_t REG_SYS_VOLTAGE_HIGH = 0x53;
  static const uint8_t REG_SYS_CURRENT_LOW = 0x70;
  static const uint8_t REG_SYS_CURRENT_HIGH = 0x71;
  static const uint8_t REG_SYS_POWER_LOW = 0x74;
  static const uint8_t REG_SYS_POWER_HIGH = 0x75;
  static const uint8_t REG_TYPEC_STATUS = 0x34;
  static const uint8_t REG_VBUS_STATUS = 0x33;

  uint8_t _intPin;          // INT引脚
  bool _isCharging;         // 充电状态
  bool _chargeComplete;     // 充电完成状态
  bool _isDischarging;      // 放电状态
  float _typeCVoltage;      // Type-C电压 (V)
  float _typeCCurrent;      // Type-C电流 (A)
  float _systemPower;       // 系统功率 (W)
  bool _typeCConnected;     // Type-C连接状态
  bool _pdCharging;         // PD快充状态
  uint16_t _voltageRaw;     // 原始电压值 (mV)
  uint16_t _currentRaw;     // 原始电流值 (mA)
  uint16_t _powerRaw;       // 原始功率值 (mW)

  // 读取充电状态
  void readChargeStatus();

  // 读取Type-C电压
  void readTypeCVoltage();

  // 读取Type-C电流
  void readTypeCCurrent();

  // 读取系统功率
  void readSystemPower();

  // 读取Type-C状态
  void readTypeCStatus();

  // 打印状态信息
  void printStatus();

  // 读取单个寄存器
  uint8_t readRegister(uint8_t regAddr);

  // 读取16位寄存器
  uint16_t read16BitRegister(uint8_t lowReg, uint8_t highReg);
  void writeRegisterWithMask(uint8_t regAddr, uint8_t value, uint8_t mask);
};
extern IP2366 ip2366;
#endif // IP2366_H