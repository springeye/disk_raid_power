//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"
#include <lvgl.h>
#include "ui_schome.h"
#include <bq40z80.h>
#include <cell_helper.h>
#include <IP2366.h>
#include <SW6306.h>
#include <log.h>
#include <temp.h>
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
    float bq_get_power()
    {
        float bq_voltage = bq_get_voltage()/1000.0f;
        float bq_current = bq_get_current()/1000.0f;
        float bq_power = bq_voltage*bq_current;
        return bq_power;
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


extern "C" {
void updateUI()
{
    // mylog.println("");
    // mylog.println("");
    // mylog.println("");
    // mylog.println("begin updateUI");

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

    if (false){
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
    }
    float ip2366_voltage =0.0f;
    float ip2366_current =0.0f;
    float ip2366_power =0.0f;
    if (ip2366.canCommunicate()) {
        // 读取所有数据并打印
        ip2366.readAllData();
        ip2366_voltage = get2366Voltage();
        ip2366_current = get2366Current();
        ip2366_power = get2366Power();
    } else {
        Serial.println("INT pin low, waiting for communication...");
    }
    float sw6306_voltage = sw.readVBUS()/1000.0f;
    float sw6306_current = sw.readIBUS()/1000.0f;
    bool is6306DisCharging=sw.isC1Source();
    bool is6306Charging=sw.isC1Sink();
    if (!is6306Charging && !is6306DisCharging)
    {
        sw6306_voltage=0.0f;
        sw6306_current=0.0f;
    }
    float sw6306_power = sw6306_voltage*sw6306_current;
    float total_out_power=0;
    float total_in_power=0;
    if (is6306Charging)
    {
        total_in_power+=sw6306_power;
    }
    if (is2366Charging())
    {
        total_in_power+=ip2366_power;
    }
    if (is6306DisCharging)
    {
        total_out_power+=sw6306_power;
    }
    if (is2366DisCharging())
    {
        total_out_power+=ip2366_power;
    }


    // sw.debugDump();
    // mylog.printf("sw6306电压:%.2f\n",sw6306_voltage);
    // mylog.printf("sw6306电流:%.2f\n",sw6306_current);
    // mylog.printf("sw6306功率:%.2f\n",sw6306_power);
    lv_label_set_text_fmt(ui_percent,"%d%%", bq_percent);
    lv_label_set_text_float(ui_power, "%sWh", bq_wh, 2);
    lv_label_set_text_float(ui_battemp, "%s°", bq_temp, 2);
    lv_label_set_text_float(ui_voltage, "%sW", bq_voltage, 1);
    lv_label_set_text_float(ui_ip2366current, "%sA", ip2366_current, 2);
    lv_label_set_text_float(ui_ip2366voltage, "%sV", ip2366_voltage, 2);
    lv_label_set_text_float(ui_ip2366power, "%sW", ip2366_power, 1);
    lv_label_set_text_float(ui_sw6306current, "%sA", sw6306_current, 2);
    lv_label_set_text_float(ui_sw6306voltage, "%sV", sw6306_voltage, 2);
    lv_label_set_text_float(ui_sw6306power, "%sW", sw6306_power, 1);
    lv_label_set_text_float(ui_outpower, "%sW", total_out_power, 1);
    lv_label_set_text_float(ui_inpower, "%sW", total_in_power, 1);
    lv_label_set_text_float(ui_boardtmp, "%s°", read_temp(), 2);
    float bat_power = bq_get_power();
    if (bat_power>0.0f)
    {
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else if (bat_power<0.0f){
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else
    {
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_label_set_text_float(ui_batpower, "%sW", bat_power, 2);
    if (is2366DisCharging())
    {
        lv_label_set_text(ui_ip2366,"OUT");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0xCB3820), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else if (is2366Charging())
    {
        lv_label_set_text(ui_ip2366,"IN");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0x2CD16C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else
    {
        lv_label_set_text(ui_ip2366,"--");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0x262525), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    if (is6306DisCharging)
    {
        lv_label_set_text(ui_sw6306,"OUT");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0xCB3820), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else if (is6306Charging)
    {
        lv_label_set_text(ui_sw6306,"IN");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0x2CD16C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    }else
    {
        lv_label_set_text(ui_sw6306,"--");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0x262525), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);

    }

    update_cells();
}
}