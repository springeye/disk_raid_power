//
// Created by develop on 2025/9/6.
//

#include "log.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <stdarg.h>

#ifdef USE_HWCDC
HWCDC USBSerial;
#endif

MyLog mylog;

MyLog::MyLog() {}

size_t MyLog::write(uint8_t c) {
#ifdef USE_HWCDC
    USBSerial.write(c);
    Serial.write(c);
#else
    Serial.write(c);
#endif
    return 1;
}

size_t MyLog::write(const uint8_t *buffer, size_t size) {
#ifdef USE_HWCDC
    USBSerial.write(buffer, size);
    Serial.write(buffer, size);
#else
    Serial.write(buffer, size);
#endif
    return size;
}

void MyLog::println(const char* msg) {
#ifdef USE_HWCDC
    USBSerial.println(msg);
    Serial.println(msg);
#else
    Serial.println(msg);
#endif
}

void MyLog::printf(const char* fmt, ...) {
    char buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
#ifdef USE_HWCDC
    USBSerial.print(buf);
    Serial.print(buf);
#else
    Serial.print(buf);
#endif
}




#endif
