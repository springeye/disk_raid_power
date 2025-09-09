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
#include <WiFi.h>

#ifdef ARDUINO
#include <log.h>
#include <Arduino.h>
#include <OneButton.h>
#include <ota.h>
#include "hal_config.h"
#ifdef ESP32
extern "C"{
    #include <esp_ota_ops.h>
    }
#include "esp_efuse.h"
#endif
#define BUTTON_PIN KEY_01

OneButton btn = OneButton(
  BUTTON_PIN,  // Input pin for the button
  true,        // Button is active LOW
  false         // Enable internal pull-up resistor
);
bool need_setup_ota = false;
char g_ip[32] = {0};
volatile bool wifi_ready = false;

void setup_ota_task(void *param) {
    setup_ota(); // WiFi初始化
    snprintf(g_ip, sizeof(g_ip), "%s", WiFi.localIP().toString().c_str());
    wifi_ready = true;
    vTaskDelete(NULL);
}
bool runSelfTest() {
    // 在这里放自检逻辑：外设初始化、传感器、存储、联网等
    // 返回 true 表示 ok，false 表示失败
    // 为演示，这里简单返回 true
    return false;
}

void checkPendingAndValidate() {
    const esp_partition_t* running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            mylog.println("New firmware PENDING_VERIFY -> run self test...");
            bool ok = runSelfTest();
            if (ok) {
                mylog.println("Self test OK -> mark app valid");
                esp_ota_mark_app_valid_cancel_rollback();
            } else {
                mylog.println("Self test FAILED -> mark invalid and reboot (roll back)");
                esp_ota_mark_app_invalid_rollback_and_reboot();
            }
        } else {
            mylog.printf("OTA state: %d\n", (int)ota_state);
        }
    } else {
        mylog.println("esp_ota_get_state_partition FAILED");
    }
}
void setup() {
#ifdef USE_HWCDC
    USBSerial.begin(9600);
    while (!USBSerial) {}
#else
    Serial.begin(9600);
    while (!Serial) {}
#endif
    delay(500);
    mylog.println("setup.....");
    // mylog.printf("%d\n",1 / 0);
    //TODO  以下代码是主动回滚OTA更新



    lv_init();
    hal_setup();
    ui_init();
    if (digitalRead(BUTTON_PIN)==LOW)
    {
        mylog.println("btn is pressed");
        lv_disp_load_scr(ui_scota);
    }else
    {
        mylog.println("btn not pressed");
        lv_disp_load_scr(ui_schome);
    }

    // btn.attachPress([]
    // {
    //     myprintln("Single Pressed!");
    // });
    btn.attachClick([]() {
        mylog.println("Single click!");
    });
    btn.attachDoubleClick([]() {
        mylog.println("Double Pressed!");
        if (lv_scr_act()==ui_schome){
            ui_scota_screen_init();
            lv_scr_load_anim(ui_scota, LV_SCR_LOAD_ANIM_MOVE_LEFT, 200, 0, true);
            xTaskCreate(setup_ota_task, "setup_ota", 4096, NULL, 1, NULL); // 启动异步任务
        }else if (lv_scr_act()==ui_scota){
            ui_schome_screen_init();
            lv_scr_load_anim(ui_schome, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, true);
            destory_ota();
        }
    });

    btn.attachLongPressStart([]() {
        mylog.println("Long Pressed start!");
    });
    btn.attachLongPressStop([]()
    {
        mylog.println("Long Pressed stop!");
    });
    checkPendingAndValidate();
}

void loop() {
    hal_loop();
    btn.tick();
    ota_loop();
    if (wifi_ready) {
        wifi_ready = false;
        lv_label_set_text(ui_Label3, g_ip); // 显示IP到页面
    }
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