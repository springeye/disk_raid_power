//
// Created by develop on 2025/9/27.
//

#ifndef DISK_RAID_POWER_KKPORTDEVICE_H
#define DISK_RAID_POWER_KKPORTDEVICE_H

#include "PortDevices.h"
class KKPortDevice : public IPortDevice {
public:
    void init() override;
    PortStatus getPortState(PortType port) override;
    uint8_t getPercent() override;
    float getTotalIn() override;
    float getTotalOut() override;
    int16_t getBatTemp() override;
    int16_t getBoardTemp() override;
    float getCellVoltage(uint8_t index) override;
    ~KKPortDevice() override;
};
#endif //DISK_RAID_POWER_KKPORTDEVICE_H