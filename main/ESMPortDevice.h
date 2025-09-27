//
// Created by develop on 2025/9/27.
//

#ifndef DISK_RAID_POWER_ESMPORTDEVICE_H
#define DISK_RAID_POWER_ESMPORTDEVICE_H

#include "PortDevices.h"
class ESMPortDevice : public IPortDevice {
public:
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

    ~ESMPortDevice() override;
};
#endif //DISK_RAID_POWER_ESMPORTDEVICE_H