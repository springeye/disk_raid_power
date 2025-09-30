//
// Created by develop on 2025/9/30.
//

#ifndef DISK_RAID_POWER_KEY_H
#define DISK_RAID_POWER_KEY_H
#include <lwbtn/lwbtn.h>
void init_btn();
void tick_btn();
uint8_t btn_get_state(struct lwbtn *lw, struct lwbtn_btn *btn);
void btn_event(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt);
#endif //DISK_RAID_POWER_KEY_H