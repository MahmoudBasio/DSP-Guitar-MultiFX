#pragma once
#include "Arduino.h"
constexpr int U8G2_R0 = 0;
constexpr int U8X8_PIN_NONE = 255;
const uint8_t u8g2_font_7x14_tr[] = {0};
const uint8_t u8g2_font_6x12_tr[] = {0};
const uint8_t u8g2_font_9x15_tr[] = {0};
const uint8_t u8g2_font_u8glib_4_tf[] = {0};
struct U8G2_SSD1306_128X64_NONAME_F_SW_I2C {
    U8G2_SSD1306_128X64_NONAME_F_SW_I2C(int, int, int, int) {}
    void begin() {}
    void clearBuffer() {}
    void setFont(const uint8_t *) {}
    void drawStr(int, int, const char *) {}
    void sendBuffer() {}
    void drawHLine(int, int, int) {}
    void drawBox(int, int, int, int) {}
    void drawFrame(int, int, int, int) {}
};
