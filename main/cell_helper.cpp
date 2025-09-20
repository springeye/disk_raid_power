//
// Created by develop on 2025/9/19.
//

#include "cell_helper.h"

#include <cstdio>
#include <monitor_api.h>
#include <stdlib_noniso.h>
#include <ui.h>
#include <ui_schome.h>

extern "C" {
    lv_obj_t* ui_cell_containers[6] = {NULL};
    lv_obj_t* ui_cells[6] = {NULL};
void lv_label_set_text_float(lv_obj_t* label, const char* fmt, float val, int decimals)
{
    char buf[32];
    dtostrf(val, 0, decimals, buf); // Arduino 自带函数
    char out[64];
    snprintf(out, sizeof(out), fmt, buf);
    lv_label_set_text(label, out);
}



void init_cells()
{

    lv_obj_set_layout(ui_cellcontainer, LV_LAYOUT_GRID);
    static const lv_coord_t col_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST
    };

    static const lv_coord_t row_dsc[] = {
        LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST
    };

    lv_obj_set_style_grid_column_dsc_array(ui_cellcontainer, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(ui_cellcontainer, row_dsc, 0);

    int i=1;
    /* 创建 6 个子对象，自动填充格子 */
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 3; ++c) {
            ui_cell_containers[i-1] =lv_obj_create(ui_cellcontainer);
            lv_obj_t *cell = ui_cell_containers[i-1];
            lv_obj_remove_style_all(cell);
            lv_obj_set_grid_cell(cell,
                                 LV_GRID_ALIGN_STRETCH, c, 1,
                                 LV_GRID_ALIGN_STRETCH, r, 1);

            lv_obj_set_flex_flow(cell, LV_FLEX_FLOW_ROW);
            lv_obj_set_flex_align(cell,
                                  LV_FLEX_ALIGN_CENTER,  // 主轴居中
                                  LV_FLEX_ALIGN_CENTER,  // 交叉轴居中
                                  LV_FLEX_ALIGN_CENTER); // 内容整体居中


            lv_obj_t *item = lv_label_create(cell);
            /* 设置文字居中对齐 */
            lv_obj_set_style_text_align(item, LV_TEXT_ALIGN_CENTER, 0);
            lv_obj_set_style_text_color(item,lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            /* 设置 label 内部内容居中（上下左右）*/
            lv_obj_set_style_pad_all(item, 0, 0);   // 去掉 padding
            lv_obj_set_style_align(item, LV_ALIGN_CENTER, 0);
            lv_obj_set_style_text_font(item, &ui_font_mibol12, LV_PART_MAIN | LV_STATE_DEFAULT);
            // float value = bq_get_cell_voltage(i)/1000.0f;
            //
            // char buf[32];
            // snprintf(buf, sizeof(buf), "%.2f", value);  // 使用 Arduino 的 snprintf
            // lv_label_set_text(item,buf);
            ui_cells[i-1] = item;
            i++;

        }
    }
}

void update_cells()
{
    for (int i = 0; i < 6; ++i) {
        float value = bq_get_cell_voltage(i+1) / 1000.0f;
        lv_label_set_text_float(ui_cells[i], "%s", value, 2);
    }
}
}
