//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"

#include <bq40z80.h>
#include <IP2366.h>
#include <log.h>
// C API 实现

extern "C" {
    uint8_t get_bq_percent(void) {
        return bq.read_RelativeStateOfCharge();
    }
    uint16_t get_bq_cell_voltage(uint8_t cell_index) {
        return bq.read_cell_voltage(cell_index);
    }
    float get_bg_remaining_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return bq.read_remaining_energy_wh(cell_count, cell_cutoff_v);
    }
    float get_bq_voltage()
    {
        return bq.read_voltage();
    }
    float get_bq_current()
    {
        return bq.read_current();
    }
}
extern "C" {
    float get_2366_voltage()
    {
        return ip2366.getTypeCVoltage();
    }
    float get_2366_current()
    {
        return ip2366.getTypeCCurrent();
    }
    float get_2366_power()
    {

        float power = ip2366.getSystemPower();
        return power;
    }
    bool is_charging_2366()
    {
        return ip2366.isCharging();
    }
    bool is_discharging_2366()
    {
        return ip2366.isDischarging();
    }
}

extern "C" {
void updateUI()
{
    float bq_voltage = get_bq_voltage()/1000.0f;
    float bq_current = get_bq_current()/1000.0f;
    float bq_power = bq_voltage*bq_current;
    float bq_wh=get_bg_remaining_wh(6,3.0f);
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

    mylog.printf("2366电压:%.2f\n",get_2366_voltage());
    mylog.printf("2366电流:%.2f\n",ip2366.getTypeCCurrent());
    mylog.printf("2366功率:%.2f\n",ip2366.getSystemPower());
}
}