#include <log.h>

#include "IP2366.h"
#include <Wire.h>

// 构造函数
IP2366::IP2366(uint8_t intPin, TwoWire* wire) : _intPin(intPin), _wire(wire) {
  // 初始化成员变量
  _isCharging = false;
  _chargeComplete = false;
  _isDischarging = false;
  _typeCVoltage = 0.0;
  _typeCCurrent = 0.0;
  _systemPower = 0.0;
  _typeCConnected = false;
  _pdCharging = false;
  _voltageRaw = 0;
  _currentRaw = 0;
  _powerRaw = 0;
}

// 初始化函数
void IP2366::begin() {
  pinMode(_intPin, INPUT);
  _wire->setClock(100000); // 建议I2C频率100kHz，符合IP2366官方建议
  delay(100);
  mylog.printf("IP2366 Monitor Initialized,PIN_INIT is %d\n",_intPin);
}

// 读取所有数据
void IP2366::readAllData() {
  if (!canCommunicate()) {
    mylog.println("Cannot communicate - INT pin is low");
    return;
  }

  readChargeStatus();
  readTypeCVoltage();
  readTypeCCurrent();
  readSystemPower();
  readTypeCStatus();
  // printStatus();
}

// 检查是否可以通信
bool IP2366::canCommunicate() {
  int read = digitalRead(_intPin);
  // mylog.printf("2366 INT pin state: %s\n", read == HIGH ? "HIGH" : "LOW");
  return read == HIGH;
}

// 获取充电状态
bool IP2366::isCharging() const {
  return _isCharging;
}

// 获取充电完成状态
bool IP2366::isChargeComplete() const {
  return _chargeComplete;
}

// 获取放电状态
bool IP2366::isDischarging() const {
  return _isDischarging;
}

// 获取Type-C输入/输出电压 (V)
float IP2366::getTypeCVoltage() const {
  return _typeCVoltage;
}

// 获取Type-C输入/输出电流 (A)
float IP2366::getTypeCCurrent() const {
  return _typeCCurrent;
}

// 获取系统功率 (W)
float IP2366::getSystemPower() const {
  return _systemPower;
}

// 获取Type-C连接状态
bool IP2366::isTypeCConnected() const {
  return _typeCConnected;
}

// 获取PD快充状态
bool IP2366::isPDCharging() const {
  return _pdCharging;
}

// 获取原始Type-C电压 (mV)
uint16_t IP2366::getTypeCVoltageRaw() const {
  return _voltageRaw;
}

// 获取原始Type-C电流 (mA)
uint16_t IP2366::getTypeCCurrentRaw() const {
  return _currentRaw;
}

// 获取原始系统功率 (mW)
uint16_t IP2366::getSystemPowerRaw() const {
  return _powerRaw;
}

// 读取充电状态
void IP2366::readChargeStatus() {
  uint8_t status = readRegister(REG_CHARGE_STATUS);
  _isCharging = (status >> 5) & 0x01;    // BIT5: CHG_En
  _chargeComplete = (status >> 4) & 0x01; // BIT4: CHG_End
  _isDischarging = (status >> 3) & 0x01;  // BIT3: Output_En
}

// 读取Type-C电压
void IP2366::readTypeCVoltage() {
  _voltageRaw = read16BitRegister(REG_SYS_VOLTAGE_LOW, REG_SYS_VOLTAGE_HIGH);
  _voltageRaw = read16BitRegister(REG_SYS_VOLTAGE_LOW, REG_SYS_VOLTAGE_HIGH);
  _typeCVoltage = _voltageRaw / 1000.0; // 转换为V
}

// 读取Type-C电流
void IP2366::readTypeCCurrent() {
  _currentRaw = read16BitRegister(REG_SYS_CURRENT_LOW, REG_SYS_CURRENT_HIGH);
  _typeCCurrent = _currentRaw / 1000.0; // 转换为A
}

// 读取系统功率
void IP2366::readSystemPower() {
  _powerRaw = read16BitRegister(REG_SYS_POWER_LOW, REG_SYS_POWER_HIGH);
  _systemPower = _typeCVoltage*_typeCCurrent; // 转换为W
}

// 读取Type-C状态
void IP2366::readTypeCStatus() {
  // 读取Type-C状态寄存器
  uint8_t typeCStatus = readRegister(REG_TYPEC_STATUS);
  _typeCConnected = (typeCStatus >> 7) & 0x01; // BIT7: Sink_Ok或Src_Ok

  // 读取VBUS状态寄存器
  uint8_t vbusStatus = readRegister(REG_VBUS_STATUS);
  _pdCharging = (vbusStatus & 0x07) > 1; // 低3位大于1表示PD快充
}

// 打印状态信息
void IP2366::printStatus() {
  mylog.println("===== IP2366 Status =====");
  mylog.print("Charging: ");
  mylog.println(_isCharging ? "Yes" : "No");
  mylog.print("Charge Complete: ");
  mylog.println(_chargeComplete ? "Yes" : "No");
  mylog.print("Discharging: ");
  mylog.println(_isDischarging ? "Yes" : "No");
  mylog.print("Type-C Connected: ");
  mylog.println(_typeCConnected ? "Yes" : "No");
  mylog.print("PD Fast Charging: ");
  mylog.println(_pdCharging ? "Yes" : "No");
  mylog.print("Type-C Voltage: ");
  mylog.print(_typeCVoltage, 3);
  mylog.println(" V");
  mylog.print("Type-C Current: ");
  mylog.print(_typeCCurrent, 3);
  mylog.println(" A");
  mylog.print("System Power: ");
  mylog.print(_systemPower, 3);
  mylog.println(" W");

  // 判断是输入还是输出
  if (_isCharging) {
    mylog.println("Mode: Type-C Input (Charging)");
  } else if (_isDischarging) {
    mylog.println("Mode: Type-C Output (Discharging)");
  } else {
    mylog.println("Mode: Standby");
  }
  mylog.println("==========================");
}

// 读取单个寄存器
uint8_t IP2366::readRegister(uint8_t regAddr) {
  _wire->beginTransmission(WRITE_ADDR);
  _wire->write(regAddr);
  uint8_t txResult = _wire->endTransmission(false);
  if (txResult != 0) {
    mylog.print("I2C NACK or error on address phase: ");
    mylog.println(txResult);
    return 0;
  }

  delayMicroseconds(50);

  int bytesRequested = _wire->requestFrom((int)READ_ADDR, 1, true);
  if (bytesRequested != 1) {
    mylog.print("I2C requestFrom failed, bytesRequested: ");
    mylog.println(bytesRequested);
    return 0;
  }
  delay(1);
  if (_wire->available()) {
    uint8_t val = _wire->read();
    delay(1);
    return val;
  }
  mylog.println("I2C read failed: no data available");
  return 0;
}


// 读取16位寄存器
uint16_t IP2366::read16BitRegister(uint8_t lowReg, uint8_t highReg) {
  // 先读取低8位寄存器
  uint8_t lowByte = readRegister(lowReg);
  delay(1); // 每个字节间延时1ms
  // 再读取高8位寄存器
  uint8_t highByte = readRegister(highReg);
  delay(1); // 每个字节间延时1ms
  // 组合成16位值
  return (highByte << 8) | lowByte;
}

// 写寄存器（读-改-写，带掩码，安全）
void IP2366::writeRegisterWithMask(uint8_t regAddr, uint8_t value, uint8_t mask) {
  // 先读原值
  uint8_t oldVal = readRegister(regAddr);
  // 按位修改
  uint8_t newVal = (oldVal & ~mask) | (value & mask);
  _wire->beginTransmission(WRITE_ADDR); // 0xEA
  _wire->write(regAddr);
  _wire->write(newVal);
  uint8_t txResult = _wire->endTransmission();
  if (txResult != 0) {
    mylog.print("I2C write error: ");
    mylog.println(txResult);
  }
  delay(1); // 写后延时，符合官方建议
}
