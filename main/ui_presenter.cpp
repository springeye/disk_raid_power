#include "ui_presenter.h"
#include <lvgl.h>
#include "ui_schome.h"
#include <cell_helper.h>
#include "data_types.h"

extern "C" {

void ui_present_all(const SystemData* data) {
    if (!data) return;

    // 数值标签更新
    lv_label_set_text_fmt(ui_percent, "%d%%", data->battery.percent);
    lv_label_set_text_float(ui_power, "%sWh", data->battery.wh, 2);
    lv_label_set_text_float(ui_battemp, "%s°", data->battery.temp, 2);
    lv_label_set_text_float(ui_voltage, "%sV", data->battery.voltage, 1);
    
    lv_label_set_text_float(ui_ip2366current, "%sA", data->portC2.current, 2);
    lv_label_set_text_float(ui_ip2366voltage, "%sV", data->portC2.voltage, 2);
    lv_label_set_text_float(ui_ip2366power, "%sW", data->portC2.power, 1);
    
    lv_label_set_text_float(ui_sw6306current, "%sA", data->portC1.current, 2);
    lv_label_set_text_float(ui_sw6306voltage, "%sV", data->portC1.voltage, 2);
    lv_label_set_text_float(ui_sw6306power, "%sW", data->portC1.power, 1);
    
    lv_label_set_text_float(ui_outpower, "%sW", data->totalOutPower, 1);
    lv_label_set_text_float(ui_inpower, "%sW", data->totalInPower, 1);
    lv_label_set_text_float(ui_boardtmp, "%s°", data->boardTemp, 2);

    // 电池功率颜色逻辑
    float bat_power = data->battery.power;
    if (bat_power > 0.0f) {
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else if (bat_power < 0.0f) {
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_obj_set_style_text_color(ui_batpower, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    lv_label_set_text_float(ui_batpower, "%sW", bat_power, 2);

    // C2 端口（IP2366）状态标签和背景颜色
    if (data->portC2.isDischarging) {
        lv_label_set_text(ui_ip2366, "OUT");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0xCB3820), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else if (data->portC2.isCharging) {
        lv_label_set_text(ui_ip2366, "IN");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0x2CD16C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_label_set_text(ui_ip2366, "--");
        lv_obj_set_style_bg_color(ui_ip2366, lv_color_hex(0x262525), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_ip2366power, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // C1 端口（SW6306）状态标签和背景颜色
    if (data->portC1.isDischarging) {
        lv_label_set_text(ui_sw6306, "OUT");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0xCB3820), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0xFAD640), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else if (data->portC1.isCharging) {
        lv_label_set_text(ui_sw6306, "IN");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0x2CD16C), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0x318BD3), LV_PART_MAIN | LV_STATE_DEFAULT);
    } else {
        lv_label_set_text(ui_sw6306, "--");
        lv_obj_set_style_bg_color(ui_sw6306, lv_color_hex(0x262525), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(ui_sw6306power, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // 调用 cell 更新
    update_cells();
}

} // extern "C"
