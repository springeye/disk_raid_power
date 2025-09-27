//
// Created by develop on 2025/9/27.
//

#ifndef DISK_RAID_POWER_PORTDEVICES_H
#define DISK_RAID_POWER_PORTDEVICES_H
#include <cstdint>

enum PortType {
    C1,
    C2,
    A1,
    A2
};

enum PortState {
    Input,
    Output,
    Standby
};

struct PortStatus {
    float voltage;
    float current;
    PortState state;
};

class IPortDevice {
public:
    virtual void init();
    virtual PortStatus getPortState(PortType port);
    virtual uint8_t getPercent() = 0;
    virtual float getTotalIn() = 0;
    virtual float getTotalOut() = 0;
    virtual int16_t getBatTemp() = 0;
    virtual int16_t getBoardTemp() = 0;
    virtual float getCellVoltage(uint8_t index) = 0;
    virtual ~IPortDevice() {}
};

#endif //DISK_RAID_POWER_PORTDEVICES_H