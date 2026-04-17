//
// i2c_utils.h — I2C 设备扫描工具
//

#ifndef DISK_RAID_POWER_I2C_UTILS_H
#define DISK_RAID_POWER_I2C_UTILS_H
#include <Wire.h>
void list_i2c_devices(TwoWire &wire, int num);
#endif // DISK_RAID_POWER_I2C_UTILS_H
