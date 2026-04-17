//
// log.h — 日志输出工具
//

#ifndef DISK_RAID_POWER_LOG_H
#define DISK_RAID_POWER_LOG_H

#ifdef ARDUINO
#include <Arduino.h>
#ifdef USE_HWCDC
#include <HWCDC.h>
extern HWCDC USBSerial;
#endif

class MyLog : public Print {
  public:
    MyLog();
    size_t write(uint8_t c) override;
    size_t write(const uint8_t *buffer, size_t size) override;
    static void println(const char *msg);
    static void println(const String &msg);
    static void println(int val);
    static void println(unsigned int val);
    static void println(long val);
    static void println(unsigned long val);
    static void println(float val);
    static void println(double val);
    static void printf(const char *fmt, ...);
    static void println(int val, int base);
    static void println(unsigned int val, int base);
};

extern MyLog mylog;
#endif

#endif // DISK_RAID_POWER_LOG_H
