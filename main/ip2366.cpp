#include <log.h>

#include "IP2366.h"
#include <Wire.h>

// 构造函数
IP2366::IP2366(uint8_t intPin) : _intPin(intPin) {
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

  // 等待系统稳定
  delay(100);
  Serial.println("IP2366 Monitor Initialized");
}

// 读取所有数据
void IP2366::readAllData() {
  if (!canCommunicate()) {
    Serial.println("Cannot communicate - INT pin is low");
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
  return digitalRead(_intPin) == HIGH;
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
  Serial.println("===== IP2366 Status =====");
  Serial.print("Charging: ");
  Serial.println(_isCharging ? "Yes" : "No");
  Serial.print("Charge Complete: ");
  Serial.println(_chargeComplete ? "Yes" : "No");
  Serial.print("Discharging: ");
  Serial.println(_isDischarging ? "Yes" : "No");
  Serial.print("Type-C Connected: ");
  Serial.println(_typeCConnected ? "Yes" : "No");
  Serial.print("PD Fast Charging: ");
  Serial.println(_pdCharging ? "Yes" : "No");
  Serial.print("Type-C Voltage: ");
  Serial.print(_typeCVoltage, 3);
  Serial.println(" V");
  Serial.print("Type-C Current: ");
  Serial.print(_typeCCurrent, 3);
  Serial.println(" A");
  Serial.print("System Power: ");
  Serial.print(_systemPower, 3);
  Serial.println(" W");

  // 判断是输入还是输出
  if (_isCharging) {
    Serial.println("Mode: Type-C Input (Charging)");
  } else if (_isDischarging) {
    Serial.println("Mode: Type-C Output (Discharging)");
  } else {
    Serial.println("Mode: Standby");
  }
  Serial.println("==========================");
}

// 读取单个寄存器
uint8_t IP2366::readRegister(uint8_t regAddr) {
  Wire.beginTransmission(WRITE_ADDR);
  Wire.write(regAddr);
  Wire.endTransmission(false);

  delayMicroseconds(50); // 文档建议的延时

  Wire.requestFrom(READ_ADDR, 1);
  delay(1); // 文档建议的字节间延时
  
  if (Wire.available()) {
    return Wire.read();
  }
  
  return 0; // 读取失败
}

// 读取16位寄存器
uint16_t IP2366::read16BitRegister(uint8_t lowReg, uint8_t highReg) {
  // 先读取低8位寄存器
  uint8_t lowByte = readRegister(lowReg);
  delay(1); // 文档建议的字节间延时
  
  // 再读取高8位寄存器
  uint8_t highByte = readRegister(highReg);
  
  // 组合成16位值
  return (highByte << 8) | lowByte;
}
// C API 实现
#include "bq40z80_c_api.h"

extern "C" {
  float get2366Voltage()
  {
    return ip2366.getTypeCVoltage();
  }
  float get2366Current()
  {
    return ip2366.getTypeCCurrent();
  }
  float get2366Power()
  {

    float power = ip2366.getSystemPower();
    mylog.print("get2366Power:");
    mylog.println(power);
    return power;
  }
  bool is2366Charging()
  {
    return ip2366.isCharging();
  }
  bool is2366DisCharging()
  {
    return ip2366.isDischarging();
  }
}
