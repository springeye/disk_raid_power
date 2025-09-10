
#include "app_hal.h"

#include <log.h>
#include <TFT_eSPI.h>

#include "lvgl.h"

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin
static const uint32_t screenWidth = TFT_WIDTH;
static const uint32_t screenHeight = TFT_HEIGHT;
TFT_eSPI tft = TFT_eSPI();

const unsigned int lvBufferSize = screenWidth * 30;
uint8_t lvBuffer[2][lvBufferSize];

static lv_display_t *lvDisplay;
static lv_indev_t *lvInput;

#if LV_USE_LOG != 0
static void lv_log_print_g_cb(lv_log_level_t level, const char *buf)
{
  LV_UNUSED(level);
  LV_UNUSED(buf);
}
#endif

/* Display flushing */
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, unsigned char *data)
{
  uint32_t w = lv_area_get_width(area);
  uint32_t h = lv_area_get_height(area);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushImageDMA(area->x1, area->y1, w, h, (uint16_t*)data);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/* Tick source, tell LVGL how much time (milliseconds) has passed */
static uint32_t my_tick(void)
{
  return millis();
}

void hal_setup(void)
{
  mylog.println("aaaaa");
  tft.begin();
  mylog.println("bbbbb");
  // tft.setRotation(1); // 根据你的屏幕调整方向
  tft.fillScreen(TFT_BLACK);
#ifdef GFX_BL
  pinMode(TFT_BLK, OUTPUT);
  digitalWrite(TFT_BLK, HIGH);
#endif


  // lv_init();
  /* Set display rotation to landscape */
  // tft.setRotation(1);

  /* Set the tick callback */
  lv_tick_set_cb(my_tick);

  /* Create LVGL display and set the flush function */
  lvDisplay = lv_display_create(screenWidth, screenHeight);
  lv_display_set_color_format(lvDisplay, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(lvDisplay, my_disp_flush);
  lv_display_set_buffers(lvDisplay, lvBuffer[0], lvBuffer[1], lvBufferSize, LV_DISPLAY_RENDER_MODE_PARTIAL);

}

void hal_loop(void)
{
  /* NO while loop in this function! (handled by framework) */
  lv_timer_handler(); // Update the UI-
  delay(5);
}
