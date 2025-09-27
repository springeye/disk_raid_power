//
// Created by develop on 2025/9/27.
//

#ifndef DISK_RAID_POWER_PORTDEVICES_H
#define DISK_RAID_POWER_PORTDEVICES_H
#include <cstdint>
#ifdef ESP32_169
enum PortType {
    C1,
    C2,
};
#elif ESP32_S3_169
enum PortType {
    C1,
    C2,
    A1,
};
#endif


enum PortState {
    Input,
    Output,
    NONE
};

struct PortStatus {
    float voltage;//V
    float current;//A
    PortState state;
};

class IPortDevice {
public:
    virtual void init()=0;
    virtual PortStatus getPortState(PortType port)=0;
    virtual uint8_t getPercent() = 0;
    virtual float getPower() = 0;
    virtual float getWh(uint8_t cell_count, float cell_cutoff_v) = 0;
    virtual float getTotalVoltage()=0;
    virtual float getTotalCurrent()=0;
    virtual float getTotalIn() = 0;
    virtual float getTotalOut() = 0;
    virtual float getBatTemp() = 0;//电池温度，单位°
    virtual float getBoardTemp() = 0;
    virtual float getCellVoltage(uint8_t index) = 0;
    virtual void loop()=0;
    virtual ~IPortDevice() {}
};

#endif //DISK_RAID_POWER_PORTDEVICES_H