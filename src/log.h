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
#endif

#ifdef __cplusplus
extern "C" {
#endif

void myprintln(const char* msg);
void myprintf(const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif //DISK_RAID_POWER_LOG_H