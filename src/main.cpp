/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "lvgl.h"
#include "app_hal.h"
#include "ui.h"

#include "demos/lv_demos.h"

#ifdef ARDUINO
#include <log.h>
#include <Arduino.h>
#include <OneButton.h>
#include <ota.h>

#define BUTTON_PIN 40

OneButton btn = OneButton(
  BUTTON_PIN,  // Input pin for the button
  true,        // Button is active LOW
  false         // Enable internal pull-up resistor
);
bool need_setup_ota = false;
void setup() {
#ifdef USE_HWCDC
    USBSerial.begin(9600);
    while (!USBSerial) {}
#else
    Serial.begin(9600);
    while (!Serial) {}
#endif
    myprintln("setup.....");
    lv_init();
    hal_setup();
    ui_init();
    // btn.attachPress([]
    // {
    //     myprintln("Single Pressed!");
    // });
    btn.attachClick([]() {
        myprintln("Single click!");
    });
    btn.attachDoubleClick([]() {
        myprintln("Double Pressed!");
        if (lv_scr_act()==ui_schome){
            ui_scota_screen_init();
            lv_scr_load_anim(ui_scota, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
            need_setup_ota = true; // 异步执行OTA
        }else if (lv_scr_act()==ui_scota){
            ui_schome_screen_init();
            lv_scr_load_anim(ui_schome, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
            destory_ota();
        }
    });

    btn.attachLongPressStart([]() {
        myprintln("Long Pressed start!");
    });
    btn.attachLongPressStop([]()
    {
        myprintln("Long Pressed stop!");
    });

}

void loop() {
    hal_loop();
    btn.tick();
    if (need_setup_ota) {
        setup_ota();
        need_setup_ota = false;
    }
    ota_loop();
}
#endif /* ARDUINO */

#ifndef ARDUINO
int main(void)
{
	lv_init();

	hal_setup();

  show_gui();

	hal_loop();
}

#endif /*ARDUINO*/