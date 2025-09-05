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
#include <Arduino.h>

void setup() {
  lv_init();
  hal_setup();
  ui_init();
}

void loop() {
  hal_loop();  /* Do not use while loop in this function */
}

#else

int main(void)
{
	lv_init();

	hal_setup();

  show_gui();

	hal_loop();
}

#endif /*ARDUINO*/