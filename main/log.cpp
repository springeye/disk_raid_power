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

void MyLog::println(int val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}

void MyLog::println(unsigned int val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}

void MyLog::println(long val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}

void MyLog::println(unsigned long val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}

void MyLog::println(float val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}

void MyLog::println(double val) {
#ifdef USE_HWCDC
    USBSerial.println(val);
    Serial.println(val);
#else
    Serial.println(val);
#endif
}





void MyLog::printf(const char* fmt, ...) {
    char buf[128]; // 缓冲区缩小为128字节
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

void MyLog::println(int val, int base) {
    char buf[32];
    switch (base) {
        case 2: {
            char binbuf[24];
            itoa(val, binbuf, 2);
            snprintf(buf, sizeof(buf), "0b%s", binbuf);
            break;
        }
        case 8:
            snprintf(buf, sizeof(buf), "0%o", val);
            break;
        case 10:
            snprintf(buf, sizeof(buf), "%d", val);
            break;
        case 16:
            snprintf(buf, sizeof(buf), "%X", val);
            break;
        default:
            snprintf(buf, sizeof(buf), "%d", val);
            break;
    }
#ifdef USE_HWCDC
    USBSerial.println(buf);
    Serial.println(buf);
#else
    Serial.println(buf);
#endif
}

void MyLog::println(unsigned int val, int base) {
    char buf[32];
    switch (base) {
        case 2: {
            char binbuf[24];
            itoa(val, binbuf, 2);
            snprintf(buf, sizeof(buf), "0b%s", binbuf);
            break;
        }
        case 8:
            snprintf(buf, sizeof(buf), "0%o", val);
            break;
        case 10:
            snprintf(buf, sizeof(buf), "%u", val);
            break;
        case 16:
            snprintf(buf, sizeof(buf), "0x%X", val);
            break;
        default:
            snprintf(buf, sizeof(buf), "%u", val);
            break;
    }
#ifdef USE_HWCDC
    USBSerial.println(buf);
    Serial.println(buf);
#else
    Serial.println(buf);
#endif
}






#endif
