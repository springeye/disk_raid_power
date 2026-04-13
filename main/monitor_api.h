//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_MONITOR_API_H
#define DISK_RAID_POWER_MONITOR_API_H
#include "PortDevices.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

// 获取电池剩余百分比
uint8_t battery_get_percent(void);
// 获取某个电芯电压
uint16_t battery_get_cell_voltage(uint8_t cell_index);
float battery_get_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v);
float battery_get_voltage();
float battery_get_current();
// 瓦
float battery_get_power();
int16_t battery_get_temp();
float port_c2_get_voltage();
float port_c2_get_current();
float port_c2_get_power();
bool port_c2_is_charging();
bool port_c2_is_discharging();
void updateUI();
#ifdef __cplusplus
}
#endif
// monitor_api.h
#ifdef __cplusplus
// 依赖注入接口 — 在 setup() 中调用 set，之后通过 get 访问
void monitor_api_set_device(IPortDevice *dev);
IPortDevice *monitor_api_get_device();
#endif

// 向后兼容别名（渐进迁移用）
#define bq_get_percent battery_get_percent
#define bq_get_cell_voltage battery_get_cell_voltage
#define bg_get_remaining_energy_wh battery_get_remaining_energy_wh
#define bq_get_voltage battery_get_voltage
#define bq_get_current battery_get_current
#define bq_get_power battery_get_power
#define bg_get_temp battery_get_temp
#define get2366Voltage port_c2_get_voltage
#define get2366Current port_c2_get_current
#define get2366Power port_c2_get_power
#define is2366Charging port_c2_is_charging
#define is2366DisCharging port_c2_is_discharging

#endif // DISK_RAID_POWER_MONITOR_API_H