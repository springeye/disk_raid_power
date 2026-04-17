//
// kk_port_device.h — KK 硬件端口设备实现
//

#ifndef DISK_RAID_POWER_KK_PORT_DEVICE_H
#define DISK_RAID_POWER_KK_PORT_DEVICE_H

#include "device/port_device.h"
#include <Wire.h>

// 前向声明驱动类，避免头文件污染
class BQ40Z80;
class SW6306;
class IP2366;

class KKPortDevice : public IPortDevice {
  public:
    // 构造函数接收外部注入的 TwoWire 实例
    explicit KKPortDevice(TwoWire *wire);
    void init() override;
    PortStatus getPortState(PortType port) override;
    uint8_t getPercent() override;
    float getPower() override;
    float getTotalIn() override;
    float getTotalOut() override;
    float getBatTemp() override;
    float getBoardTemp() override;
    float getCellVoltage(uint8_t index) override;
    float getWh(uint8_t cell_count, float cell_cutoff_v) override;
    float getTotalVoltage() override;
    float getTotalCurrent() override;
    void loop() override;

    ~KKPortDevice() override;

  private:
    PortStatus buildPortStatus(bool isCharging, bool isDischarging, float voltage, float current) const;
    TwoWire *_wire; // 外部注入，不 delete
    BQ40Z80 *_bq = nullptr;
    SW6306 *_sw = nullptr;
    IP2366 *_ip2366 = nullptr;
};
#endif // DISK_RAID_POWER_KK_PORT_DEVICE_H
