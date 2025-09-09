//
// Created by develop on 2025/9/6.
//

#ifndef DISK_RAID_POWER_LOG_H
#define DISK_RAID_POWER_LOG_H

#ifdef ARDUINO
#include <Arduino.h>
#ifdef USE_HWCDC
#include <HWCDC.h>
extern HWCDC USBSerial;
#endif

class MyLog : public Print
{
public:
    MyLog();
    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buffer, size_t size) override;
    void println(const char* msg);
    void printf(const char* fmt, ...);
};

extern MyLog mylog;
#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif //DISK_RAID_POWER_LOG_H
