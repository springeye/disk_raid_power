
#include <Arduino_GFX_Library.h>
#include "axs5106l_device.h"
#include <Wire.h>
/*
 ** LCD_CS:   9
 ** LCD_DC:   8
 ** LCD_RST:  7
 ** LCD_BL:   6
 ** SCLK:     13
 ** MOSI:     11
 ** MISO:     12
*/

#define GFX_BL 6  // default backlight pin, you may replace DF_GFX_BL to actual

#define TOUCH_RST_PIN 2
#define TOUCH_INT_PIN 3


/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ST7796(
  bus, DF_GFX_RST /* RST */, 0 /* rotation */, false /* IPS */,
  172 /* width */, 320 /* height */,
  34 /*col_offset1*/, 0 /*uint8_t row_offset1*/,
  34 /*col_offset2*/, 0 /*row_offset2*/);

void LCD_display_test(void) {
  gfx->fillScreen(RED);
  delay(1000);  // 1 seconds
  gfx->fillScreen(GREEN);
  delay(1000);  // 1 seconds
  gfx->fillScreen(BLUE);
  delay(1000);  // 1 seconds
  gfx->fillScreen(WHITE);
}

void setup(void) {
  Serial.begin(115200);
  Wire.begin();

  touch_init(&Wire, TOUCH_RST_PIN, TOUCH_INT_PIN);
  Serial.println("Arduino_GFX Hello World example");

  // Init Display
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  bus->beginWrite();
  bus->writeC8D8(0x36, 0x48);
  bus->endWrite();
  gfx->fillScreen(WHITE);

#ifdef GFX_BL
  pinMode(GFX_BL, OUTPUT);
  digitalWrite(GFX_BL, HIGH);
#endif
  gfx->setCursor(0, 30);
  gfx->setTextSize(2);
  gfx->setTextColor(RED);
  gfx->println("Hello\n");
  gfx->println("Waveshare");
  delay(2000);  // 2 seconds
  LCD_display_test();
}
touch_data_t touch_data;
void loop() {
  if (get_touch_data(&touch_data)) {
    touch_data.coords[0].x = gfx->width() - 1 - touch_data.coords[0].x;
    Serial.print("x:");
    Serial.print(touch_data.coords[0].x);
    Serial.print("y:");
    Serial.println(touch_data.coords[0].y);
    gfx->drawCircle(touch_data.coords[0].x, touch_data.coords[0].y, 2, RED);
  }
  delay(10);  // 1 second
}
