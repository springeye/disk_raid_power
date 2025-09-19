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

extern "C" {
void updateUI()
{
    mylog.printf("BQ当前电压:%.3fV\n",bq.read_voltage()/1000.0f);
    mylog.printf("BQ当前电流:%.3FA\n",bq.read_current()/1000.0f);
    mylog.printf("BQ当前功率:%.3FW\n",bq.read_current()/1000.0f*bq.read_voltage()/1000.0f);
    mylog.printf("BQ充电中:%s\n",bq.is_charging());
    mylog.printf("BQ放电中:%s\n",bq.is_discharging());
    mylog.printf("BQ当前电量:%d%%\n",bq.read_capacity());
    mylog.printf("BQ当前容量:%.2fWh\n",bq.read_remaining_energy_wh(6,3.0f));
    mylog.printf("BQ当前温度:%.2f°\n",bq.read_temp()/10.0f);
    mylog.printf("BQ电芯1:%d\n",bq.read_cell_voltage(1));
    mylog.printf("BQ电芯2:%d\n",bq.read_cell_voltage(2));
    mylog.printf("BQ电芯3:%d\n",bq.read_cell_voltage(3));
    mylog.printf("BQ电芯4:%d\n",bq.read_cell_voltage(4));
    mylog.printf("BQ电芯5:%d\n",bq.read_cell_voltage(5));
    mylog.printf("BQ电芯6:%d\n",bq.read_cell_voltage(6));

    mylog.printf("2366电压:%.2f\n",ip2366.getTypeCVoltage());
    mylog.printf("2366电流:%.2f\n",ip2366.getTypeCCurrent());
    mylog.printf("2366功率:%.2f\n",ip2366.getSystemPower());
}
}