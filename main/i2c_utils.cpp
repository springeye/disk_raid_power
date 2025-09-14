//
// Created by develop on 2025/6/20.
//

#include "i2c_utils.h"

#include <Arduino.h>
#include <log.h>

void list_i2c_devices(TwoWire &wire,int num=1) {

    byte error, address;
    int nDevices = 0;
    //
    for(address = 1; address < 127; address++) {
        wire.beginTransmission(address);
        error = wire.endTransmission();

        if (error == 0) {
            mylog.printf("I2C%d device found at 0x",num);
            mylog.println(address, HEX);
            nDevices++;
        }
    }

    if (nDevices == 0) {
        mylog.printf("No I2C%d devices found\n",num);
    }

}
