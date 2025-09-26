//
// Created by develop on 2025/9/26.
//

#ifndef DISK_RAID_POWER_ARDUINO_ADAPTER_H
#define DISK_RAID_POWER_ARDUINO_ADAPTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
uint8_t ASYNC_I2C_Transmit(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag);
uint8_t ASYNC_I2C_Receive(uint8_t addr, uint8_t reg, uint8_t *pdata, uint8_t len, uint8_t *pflag);

#ifdef __cplusplus
}
#endif

#endif //DISK_RAID_POWER_ARDUINO_ADAPTER_H