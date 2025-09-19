//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"
#include <lvgl.h>
#include "ui_schome.h"
#include <bq40z80.h>
#include <IP2366.h>
#include <log.h>
// C API 实现

extern "C" {
    uint8_t bq_get_percent(void) {
        return bq.read_capacity();
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
    int16_t bg_get_temp()
    {
        return bq.read_temp();
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
void lv_label_set_text_float(lv_obj_t* label, const char* fmt, float val, int decimals) {
    char buf[32];
    dtostrf(val, 0, decimals, buf);   // Arduino 自带函数
    char out[64];
    snprintf(out, sizeof(out), fmt, buf);
    lv_label_set_text(label, out);
}

extern "C" {
void updateUI()
{
    if (ip2366.canCommunicate()) {
        // 读取所有数据并打印
        ip2366.readAllData();
    } else {
        Serial.println("INT pin low, waiting for communication...");
    }
    float bq_voltage = bq_get_voltage()/1000.0f;
    float bq_current = bq_get_current()/1000.0f;
    float bq_power = bq_voltage*bq_current;
    float bq_wh=bg_get_remaining_energy_wh(6,3.0f);
    uint8_t bq_percent=bq_get_percent();
    float bq_temp=static_cast<float>(bg_get_temp())/10.0f;
    bool bq_is_charging = bq.is_charging();
    bool bq_is_discharging = bq.is_discharging();
    float cell[6] = {0.0f};
    for (int i = 0; i < 6; ++i) {
        cell[i] = static_cast<float>(bq.read_cell_voltage(i + 1)/1000);
    }

    mylog.printf("BQ当前电压:%.3fV\n",bq_voltage);
    mylog.printf("BQ当前电流:%.3FA\n",bq_current);
    mylog.printf("BQ当前功率:%.3FW\n",bq_power);
    mylog.printf("BQ当前温度:%.2f°\n",bq_temp);
    mylog.printf("BQ充电中:%d\n",bq_is_charging);
    mylog.printf("BQ放电中:%d\n",bq_is_discharging);
    mylog.printf("BQ当前电量:%d%%\n",bq_percent);
    mylog.printf("BQ当前容量:%.2fWh\n",bq_wh);



    mylog.printf("BQ电芯1:%.3f\n",cell[0]);
    mylog.printf("BQ电芯2:%.3f\n",cell[1]);
    mylog.printf("BQ电芯3:%.3f\n",cell[2]);
    mylog.printf("BQ电芯4:%.3f\n",cell[3]);
    mylog.printf("BQ电芯5:%.3f\n",cell[4]);
    mylog.printf("BQ电芯6:%.3f\n",cell[5]);


    float ip2366_voltage = get2366Voltage();
    float ip2366_current = get2366Current();
    float ip2366_power = get2366Power();
    mylog.printf("2366电压:%.2f\n",ip2366_voltage);
    mylog.printf("2366电流:%.2f\n",ip2366_current);
    mylog.printf("2366功率:%.2f\n",ip2366_power);

    lv_label_set_text_fmt(ui_percent,"%d%%", bq_percent);
    lv_label_set_text_float(ui_power, "%sWh", bq_wh, 2);
    lv_label_set_text_float(ui_bat_temp, "%s°", bq_temp, 2);
    lv_label_set_text_float(ui_2366_current, "%sA", ip2366_current, 2);
    lv_label_set_text_float(ui_2366_voltage, "%sV", ip2366_voltage, 2);
    lv_label_set_text_float(ui_2366_power, "%sW", ip2366_power, 2);

}
}