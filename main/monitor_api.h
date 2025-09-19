//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_MONITOR_API_H
#define DISK_RAID_POWER_MONITOR_API_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

    // 获取电池剩余百分比
    uint8_t get_bq_percent(void);
    // 获取某个电芯电压
    uint16_t get_bq_cell_voltage(uint8_t cell_index);
    float get_bg_remaining_wh(uint8_t cell_count, float cell_cutoff_v);
    float get_bq_voltage();
    float get_bq_current();
    float get_2366_voltage();
    float get_2366_current();
    float get_2366_power();
    bool is_charging_2366();
    bool is_discharging_2366();
    void updateUI();
#ifdef __cplusplus
}
#endif

#endif //DISK_RAID_POWER_MONITOR_API_H