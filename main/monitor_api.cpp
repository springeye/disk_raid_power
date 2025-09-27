//
// Created by develop on 2025/9/19.
//

#include "monitor_api.h"
#include <lvgl.h>
#include "ui_schome.h"
#include <cell_helper.h>
#include <log.h>
#include <temp.h>
#if defined(ESP32_169)
#include <KKPortDevice.h>
IPortDevice* device = new KKPortDevice();
#elif defined(ESP32_S3_169)
#include <ESMPortDevice.h>
IPortDevice* device = new ESMPortDevice();
#endif

extern "C" {
    uint8_t bq_get_percent(void) {
        return device->getPercent();
    }
    uint16_t bq_get_cell_voltage(uint8_t cell_index) {
        return device->getCellVoltage(cell_index);
    }
    float bg_get_remaining_energy_wh(uint8_t cell_count, float cell_cutoff_v)
    {
        return device->getWh(cell_count, cell_cutoff_v);
    }
    float bq_get_voltage()
    {
        return device->getTotalVoltage();
    }
    float bq_get_current()
    {
        return device->getTotalCurrent();
    }
    float bq_get_power()
    {

        return device->getPower();
    }
    int16_t bg_get_temp()
    {
        return device->getBatTemp();
    }
}
extern "C" {
    float get2366Voltage()
    {
        return device->getPortState(PortType::C2).voltage;
    }
    float get2366Current()
    {
        return device->getPortState(PortType::C2).current;
    }
    float get2366Power()
    {
        auto status = device->getPortState(PortType::C2);
        return status.voltage*status.current;
    }
    bool is2366Charging()
    {
        return device->getPortState(PortType::C2).state==PortState::Input;
    }
    bool is2366DisCharging()
    {
        return device->getPortState(PortType::C2).state==PortState::Output;
    }
}


extern "C" {
void updateUI()
{
    device->loop();
    float bq_voltage = bq_get_voltage()/1000.0f;
    float bq_current = bq_get_current()/1000.0f;
    float bq_wh=bg_get_remaining_energy_wh(6,3.0f);
    uint8_t bq_percent=bq_get_percent();
    float bq_power = bq_get_power();

    float bq_temp=bg_get_temp();
    mylog.printf("updateUi:percent=%d%%\n",bq_percent);
    mylog.printf("updateUi:power=%.2fW\n",bq_power);
    mylog.printf("updateUi:voltage=%.2fV\n",bq_voltage);
    mylog.printf("updateUi:current=%.2fA\n",bq_current);
    mylog.printf("updateUi:wh=%.2fWh\n",bq_wh);
    mylog.printf("updateUi:temp=%.2f°\n",bq_temp);
    float cell[6] = {0.0f};
    for (int i = 0; i < 6; ++i) {
        cell[i] = device->getCellVoltage(i + 1)/1000;
    }


    float ip2366_voltage = get2366Voltage();
    float ip2366_current =get2366Current();
    float ip2366_power =get2366Power();

    float sw6306_voltage = device->getPortState(PortType::C1).voltage;
    float sw6306_current = device->getPortState(PortType::C1).current;
    bool is6306DisCharging=device->getPortState(PortType::C1).state==PortState::Output;
    bool is6306Charging=device->getPortState(PortType::C1).state==PortState::Input;
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


    lv_label_set_text_fmt(ui_percent,"%d%%", bq_percent);
    lv_label_set_text_float(ui_power, "%sWh", bq_wh, 2);
    lv_label_set_text_float(ui_battemp, "%s°", bq_temp, 2);
    lv_label_set_text_float(ui_voltage, "%sV", bq_voltage, 1);
    lv_label_set_text_float(ui_ip2366current, "%sA", ip2366_current, 2);
    lv_label_set_text_float(ui_ip2366voltage, "%sV", ip2366_voltage, 2);
    lv_label_set_text_float(ui_ip2366power, "%sW", ip2366_power, 1);
    lv_label_set_text_float(ui_sw6306current, "%sA", sw6306_current, 2);
    lv_label_set_text_float(ui_sw6306voltage, "%sV", sw6306_voltage, 2);
    lv_label_set_text_float(ui_sw6306power, "%sW", sw6306_power, 1);
    lv_label_set_text_float(ui_outpower, "%sW", total_out_power, 1);
    lv_label_set_text_float(ui_inpower, "%sW", total_in_power, 1);
    lv_label_set_text_float(ui_boardtmp, "%s°", device->getBoardTemp(), 2);
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