#include "sw6306v.h"
#include <Wire.h>

/**
 * @brief 构造函数
 * @param address SW6306V的I2C地址
 */
SW6306V_PowerMonitor::SW6306V_PowerMonitor(uint8_t address) : _address(address) {}

/**
 * @brief 初始化I2C连接
 * @param sda SDA引脚
 * @param scl SCL引脚
 * @return true: 初始化成功, false: 初始化失败
 */
bool SW6306V_PowerMonitor::begin(uint8_t sda, uint8_t scl) {
    // 初始化I2C总线
    Wire.begin(sda, scl);
    // 设置I2C速度为100kHz (SW6306V支持100kHz/400kHz)
    Wire.setClock(100000);

    // 尝试读取芯片版本验证连接
    uint8_t version = readRegister(REG_CHIP_VERSION);
    return (version != 0xFF);
}

/**
 * @brief 解锁I2C写操作 (必须首先调用)
 * @return true: 解锁成功, false: 解锁失败
 */
bool SW6306V_PowerMonitor::unlock() {
    // 根据数据手册的解锁流程

    // Step 1: 禁用低功耗模式 (REG0x23 bit0 = 1)
    if (!writeRegister(REG_LOW_POWER_CTRL, 0x01)) {
        return false; // 写入失败
    }
    delay(5); // 短延时确保写入生效

    // Step 2: 发送解锁序列 0x20 -> 0x40 -> 0x80
    if (!writeRegister(REG_WRITE_UNLOCK, 0x20)) {
        return false;
    }
    delay(1);

    if (!writeRegister(REG_WRITE_UNLOCK, 0x40)) {
        return false;
    }
    delay(1);

    if (!writeRegister(REG_WRITE_UNLOCK, 0x80)) {
        return false;
    }
    delay(5); // 解锁后建议稍长延时

    return true;
}

/**
 * @brief 读取系统状态 (输入/输出状态)
 * @param[out] isInput 是否为输入状态 (充电)
 * @param[out] isOutput 是否为输出状态 (放电)
 * @return true: 读取成功, false: 读取失败
 */
std::optional<SystemStatus> SW6306V_PowerMonitor::getSystemStatus() {
    uint8_t status = readRegister(REG_SYS_STATUS);
    if (status == 0xFF) {
        return std::nullopt;
    }
    SystemStatus result;
    result.isInput = (status & 0b00100000) != 0;
    result.isOutput = (status & 0b00010000) != 0;
    return result;
}

/**
 * @brief 获取充电电压信息
 * @param[out] actualVoltage 实际充电电压 (单位: mV)
 * @param[out] requestedVoltage 请求的充电电压 (单位: mV)
 * @return true: 读取成功, false: 读取失败
 */
std::optional<ChargingVoltage> SW6306V_PowerMonitor::getChargingVoltage() {
    uint8_t status = readRegister(REG_CHG_VOLT_STATUS);
    if (status == 0xFF) {
        return std::nullopt;
    }
    ChargingVoltage result;
    uint8_t actualIdx = (status >> 4) & 0b00000111;
    switch (actualIdx) {
        case 0: result.actualVoltage = 5000; break;
        case 1: result.actualVoltage = 9000; break;
        case 2: result.actualVoltage = 10000; break;
        case 3: result.actualVoltage = 12000; break;
        case 4: result.actualVoltage = 15000; break;
        case 5: result.actualVoltage = 20000; break;
        default: result.actualVoltage = 0; break;
    }
    uint8_t requestedIdx = status & 0b00000111;
    switch (requestedIdx) {
        case 0: result.requestedVoltage = 5000; break;
        case 1: result.requestedVoltage = 9000; break;
        case 2: result.requestedVoltage = 10000; break;
        case 3: result.requestedVoltage = 12000; break;
        case 4: result.requestedVoltage = 15000; break;
        case 5: result.requestedVoltage = 20000; break;
        default: result.requestedVoltage = 0; break;
    }
    return result;
}

/**
 * @brief 获取充电协议信息
 * @param[out] protocolName 协议名称字符串
 * @param[out] isFastCharge 是否为快充协议
 * @return true: 读取成功, false: 读取失败
 */
std::optional<ChargingProtocol> SW6306V_PowerMonitor::getChargingProtocol() {
    uint8_t status = readRegister(REG_FAST_CHG_IND);
    if (status == 0xFF) {
        return std::nullopt;
    }
    ChargingProtocol result;
    result.isFastCharge = (status & 0b00010000) != 0;
    uint8_t protocolIdx = status & 0b00001111;
    switch (protocolIdx) {
        case 0: result.protocolName = "None"; break;
        case 1: result.protocolName = "QC2.0"; break;
        case 2: result.protocolName = "QC3.0"; break;
        case 3: result.protocolName = "QC3.0+"; break;
        case 4: result.protocolName = "FCP"; break;
        case 5: result.protocolName = "SCP"; break;
        case 6: result.protocolName = "PD Fixed"; break;
        case 7: result.protocolName = "PD PPS"; break;
        case 8: result.protocolName = "PE1.1"; break;
        case 9: result.protocolName = "PE2.0"; break;
        case 10: result.protocolName = "VOOC1.0"; break;
        case 11: result.protocolName = "VOOC4.0"; break;
        case 12: result.protocolName = "SuperVOOC"; break;
        case 13: result.protocolName = "SFCP"; break;
        case 14: result.protocolName = "AFC"; break;
        case 15: result.protocolName = "UFCS"; break;
        default: result.protocolName = "Unknown"; break;
    }
    return result;
}

/**
 * @brief 获取完整的充电状态信息
 * @param[out] isInput 是否为输入状态
 * @param[out] isOutput 是否为输出状态
 * @param[out] actualVoltage 实际充电电压 (mV)
 * @param[out] inputCurrent 输入电流 (mA)
 * @param[out] protocolName 协议名称
 * @return true: 读取成功, false: 读取失败
 */
std::optional<ChargingStatus> SW6306V_PowerMonitor::getChargingStatus() {
    auto sysOpt = getSystemStatus();
    if (!sysOpt.has_value()) return std::nullopt;
    auto voltOpt = getChargingVoltage();
    if (!voltOpt.has_value()) return std::nullopt;
    uint16_t inputCurrent = getInputCurrent();
    if (inputCurrent == 0xFFFF) return std::nullopt;
    auto protocolOpt = getChargingProtocol();
    if (!protocolOpt.has_value()) return std::nullopt;
    ChargingStatus result;
    result.sys = sysOpt.value();
    result.volt = voltOpt.value();
    result.inputCurrent = inputCurrent;
    result.protocol = protocolOpt.value();
    return result;
}
/**
 * @brief 获取输入电流
 * @return 输入电流 (单位: mA, 0xFFFF表示读取失败)
 */
uint16_t SW6306V_PowerMonitor::getInputCurrent()
{
    // 读取输入电流寄存器 (REG0x3A和REG0x3B)
    uint8_t msb = readRegister(REG_IN_CURRENT_MSB);
    uint8_t lsb = readRegister(REG_IN_CURRENT_LSB);

    if (msb == 0xFF || lsb == 0xFF) {
        return 0xFFFF; // 读取失败
    }

    // 计算实际电流值 (单位: mA)
    // 根据数据手册，ADC值为12位，LSB = 2mA
    uint16_t adcValue = ((msb << 8) | lsb) & 0b0000111111111111;
    return adcValue * 2; // 转换为mA
}
/**
 * @brief 打印完整的充电状态信息到串口
 */
void SW6306V_PowerMonitor::printChargingStatus() {
    auto statusOpt = getChargingStatus();
    if (!statusOpt.has_value()) {
        Serial.println("Error reading charging status!");
        return;
    }
    const ChargingStatus &status = statusOpt.value();
    Serial.println("\n=== SW6306V Power Status ===");
    Serial.print("Mode: ");
    if (status.sys.isInput) {
        Serial.println("INPUT (Charging)");
    } else if (status.sys.isOutput) {
        Serial.println("OUTPUT (Discharging)");
    } else {
        Serial.println("STANDBY");
    }
    Serial.print("Voltage: ");
    Serial.print(status.volt.actualVoltage / 1000.0, 1);
    Serial.println("V");
    Serial.print("Input Current: ");
    Serial.print(status.inputCurrent);
    Serial.println("mA");
    Serial.print("Protocol: ");
    Serial.println(status.protocol.protocolName);
    Serial.println("============================");
}

/**
 * @brief 读取单个寄存器
 * @param reg 寄存器地址
 * @return 寄存器值 (0xFF表示读取失败)
 */
uint8_t SW6306V_PowerMonitor::readRegister(uint8_t reg) const {
    // 开始传输，指定设备地址
    Wire.beginTransmission(_address);
    // 发送要读取的寄存器地址
    Wire.write(reg);

    // 结束传输但不发送停止条件 (保持连接)
    if (Wire.endTransmission(false) != 0) {
        return 0xFF; // 传输失败
    }

    // 请求1字节数据
    if (Wire.requestFrom(_address, (uint8_t)1) != 1) {
        return 0xFF; // 读取失败
    }

    // 读取并返回数据
    return Wire.read();
}

/**
 * @brief 写入单个寄存器
 * @param reg 寄存器地址
 * @param data 要写入的数据
 * @return true: 写入成功, false: 写入失败
 */
bool SW6306V_PowerMonitor::writeRegister(uint8_t reg, uint8_t data) const {
    // 开始传输，指定设备地址
    Wire.beginTransmission(_address);
    // 发送寄存器地址
    Wire.write(reg);
    // 发送要写入的数据
    Wire.write(data);

    // 结束传输并检查结果
    return (Wire.endTransmission() == 0);
}