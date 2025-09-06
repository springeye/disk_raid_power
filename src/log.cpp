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

void myprintln(const char* msg) {
#ifdef USE_HWCDC
    USBSerial.println(msg);
    Serial.println(msg);
#else
    Serial.println(msg);
#endif
}

void myprintf(const char* fmt, ...) {
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
