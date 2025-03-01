#pragma once
#include <U8g2lib.h>
#include "HUD.hpp"

const uint8_t CENTER_X = 64;
const uint8_t CENTER_Y = 64;

const uint8_t FONT_X = 29;
const uint8_t FONT_Y = 100;

const uint8_t RPM_ARC_RADIUS = 53;
const uint8_t RPM_ARC_START = 20;
const uint8_t RPM_ARC_END = 108;
const uint8_t RPM_ARC_THICKNESS = 3;

const uint16_t MAX_RPM = 8000;

class SH1107HUD: HUD<U8G2_SH1107_128X128_F_HW_I2C>
{
private:
    void draw_speed(uint8_t speed);
    void draw_rpm(uint16_t rpm);
public:
    SH1107HUD();
    void begin();
    void draw(uint8_t speed, uint16_t rpm);
};
