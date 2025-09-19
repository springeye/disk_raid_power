//
// Created by develop on 2025/9/19.
//

#ifndef DISK_RAID_POWER_CELL_HELPER_H
#define DISK_RAID_POWER_CELL_HELPER_H
#include <lvgl.h>
#ifdef __cplusplus
extern "C" {
#endif
    extern lv_obj_t* ui_cell_containers[6];
    extern lv_obj_t* ui_cells[6];
void lv_label_set_text_float(lv_obj_t* label, const char* fmt, float val, int decimals);
void init_cells();
void update_cells();
#ifdef __cplusplus
}
#endif
#endif //DISK_RAID_POWER_CELL_HELPER_H