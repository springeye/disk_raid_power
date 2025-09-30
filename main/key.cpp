//
// Created by develop on 2025/9/30.
//

#include "key.h"

#include <cstdint>
#include <Arduino.h>
#include <log.h>
#include <lwbtn/lwbtn.h>
static lwbtn_btn_t btns[] = {
        {.arg = (uint8_t)KEY_01}
};
void init_btn()
{
    pinMode(KEY_01, INPUT);
    lwbtn_init_ex(NULL, btns, std::size(btns),btn_get_state, btn_event);
}
void tick_btn()
{
     lwbtn_process_ex(NULL, millis());
}

uint8_t btn_get_state(struct lwbtn* lw, struct lwbtn_btn* btn)
{
    return (digitalRead(btn->arg) == LOW);
}

void btn_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt)
{
    switch (evt)
    {
    case LWBTN_EVT_ONPRESS:         {
            mylog.println("key press!");
            mylog.printf("last_state:%d\n",btn->last_state);
            mylog.printf("time_change:%d\n",btn->time_change);
            mylog.printf("time_state_change:%d\n",btn->time_state_change);

            break;
    }
    case LWBTN_EVT_ONRELEASE:        {
            mylog.println("key release!");
            break;
    }
    case LWBTN_EVT_ONCLICK:
        {
            // mylog.println("Single click!");
            break;
        }
    case LWBTN_EVT_KEEPALIVE:        {
            mylog.println("key keepalive");
            mylog.printf("keepalive.cnt:%d\n",btn->keepalive.cnt);
            mylog.printf("keepalive.last_time:%d\n",btn->keepalive.last_time);
            break;
    }
    default: break;
    }
}
/**
btn.attachDoubleClick([]()
        {
            mylog.println("Double Pressed!");
            if (lv_scr_act() == ui_schome)
            {
                ui_scota_screen_init();
                lv_scr_load_anim(ui_scota, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
                xTaskCreate(setup_ota_task, "setup_ota", 4096, NULL, 1, NULL); // 启动异步任务
            }
            else if (lv_scr_act() == ui_scota)
            {
                ui_schome_screen_init();
                lv_scr_load_anim(ui_schome, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
                destory_ota();
            }
        });
        btn.attachLongPressStart([]()
        {
            mylog.println("Long Pressed start!");
            digitalWrite(12, LOW); // 默认拉高（符合大多数硬件需求）
        });
        btn.attachLongPressStop([]()
        {
            mylog.println("Long Pressed stop!");
        });
 */