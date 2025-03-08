#include "SH1107HUD.hpp"

SH1107HUD::SH1107HUD()
{
    this->display = new U8G2_SH1107_128X128_F_HW_I2C(U8G2_MIRROR);
    this->display->begin();
    this->display->setContrast(255);
    this->draw(0, 0);
}

void SH1107HUD::draw_speed(uint8_t speed)
{
    this->display->setFont(u8g2_font_fub30_tn);
    this->display->drawStr(FONT_X, FONT_Y, u8x8_u8toa(speed, 3));

    this->display->setFont(u8g2_font_resoledbold_tr);
    this->display->drawStr(85, 110, "km/h");
}

void SH1107HUD::draw_rpm(uint16_t rpm)
{
    this->display->drawArc(CENTER_X, CENTER_Y, RPM_ARC_RADIUS, map(rpm, 0, MAX_RPM, RPM_ARC_END - 1, RPM_ARC_START), RPM_ARC_END, RPM_ARC_THICKNESS);
    this->display->setFont(u8g2_font_fub11_tr);
    this->display->drawStr(CENTER_X - 18, CENTER_Y - 25, u8x8_u16toa(rpm, 4));
    this->display->setFont(u8g2_font_resoledbold_tr);
    this->display->drawStr(CENTER_X + 22, CENTER_Y - 18, "rpm");
}

void SH1107HUD::draw(uint8_t speed, uint16_t rpm)
{
    this->display->clearBuffer();
    this->draw_speed(speed);
    this->draw_rpm(rpm);
    this->display->sendBuffer();
}