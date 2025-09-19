//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"

#include <bq40z80.h>
#include <IP2366.h>
#include <log.h>
// C API 实现

extern "C" {
    uint8_t bq_get_percent(void) {
        return bq.read_RelativeStateOfCharge();
    }
    uint16_t bq_get_cell_voltage(uint8_t cell_index) {
        return bq.read_cell_voltage(cell_index);
    }
    float read_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return bq.read_remaining_energy_wh(cell_count, cell_cutoff_v);
    }
    float read_voltage()
    {
        return bq.read_voltage();
    }

}
extern "C" {
    float get2366Voltage()
    {
        return ip2366.getTypeCVoltage();
    }
    float get2366Current()
    {
        return ip2366.getTypeCCurrent();
    }
    float get2366Power()
    {

        float power = ip2366.getSystemPower();
        return power;
    }
    bool is2366Charging()
    {
        return ip2366.isCharging();
    }
    bool is2366DisCharging()
    {
        return ip2366.isDischarging();
    }
}

