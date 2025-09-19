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
    float bg_get_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return bq.read_remaining_energy_wh(cell_count, cell_cutoff_v);
    }
    float bq_get_voltage()
    {
        return bq.read_voltage();
    }
    float bq_get_current()
    {
        return bq.read_current();
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

extern "C" {
void updateUI()
{
    float bq_voltage = bq_get_voltage()/1000.0f;
    float bq_current = bq_get_current()/1000.0f;
    float bq_power = bq_voltage*bq_current;
    float bq_wh=bg_get_remaining_energy_wh(6,3.0f);
    float bq_temp=static_cast<float>(bq.read_temp())/10.0f;
    bool bq_is_charging = bq.is_charging();
    bool bq_is_discharging = bq.is_discharging();
    float cell[6] = {0.0f};
    for (int i = 0; i < 6; ++i) {
        cell[i] = static_cast<float>(bq.read_cell_voltage(i + 1)/1000);
    }



    mylog.printf("BQ当前电压:%.3fV\n",bq_voltage);
    mylog.printf("BQ当前电流:%.3FA\n",bq_current);
    mylog.printf("BQ当前功率:%.3FW\n",bq_power);
    mylog.printf("BQ充电中:%s\n",bq_is_charging);
    mylog.printf("BQ放电中:%s\n",bq_is_discharging);
    mylog.printf("BQ当前电量:%d%%\n",bq.read_capacity());
    mylog.printf("BQ当前容量:%.2fWh\n",bq_wh);
    mylog.printf("BQ当前温度:%.2f°\n",bq_temp);


    mylog.printf("BQ电芯1:%d\n",cell[0]);
    mylog.printf("BQ电芯2:%d\n",cell[1]);
    mylog.printf("BQ电芯3:%d\n",cell[2]);
    mylog.printf("BQ电芯4:%d\n",cell[3]);
    mylog.printf("BQ电芯5:%d\n",cell[4]);
    mylog.printf("BQ电芯6:%d\n",cell[5]);

    mylog.printf("2366电压:%.2f\n",ip2366.getTypeCVoltage());
    mylog.printf("2366电流:%.2f\n",ip2366.getTypeCCurrent());
    mylog.printf("2366功率:%.2f\n",ip2366.getSystemPower());
}
}