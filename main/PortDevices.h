//
// Created by develop on 2025/9/27.
//

#ifndef DISK_RAID_POWER_PORTDEVICES_H
#define DISK_RAID_POWER_PORTDEVICES_H
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
    virtual PortStatus getPortState(PortType port);
    virtual float getPercent() = 0;
    virtual float getTotalIn() = 0;
    virtual float getTotalOut() = 0;
    virtual float getBatTemp() = 0;
    virtual float getBoardTemp() = 0;
    virtual float getCellVoltage(uint_t index) = 0;
    virtual ~IPortDevice() {}
};

#endif //DISK_RAID_POWER_PORTDEVICES_H