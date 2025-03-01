#pragma once
#include <cstdint>

template <typename T>
class HUD
{
protected:
    T* display;
    virtual void draw_speed(uint8_t speed) = 0;
    virtual void draw_rpm(uint16_t rpm) = 0;
public:
    virtual void begin() = 0;
    virtual void draw(uint8_t speed, uint16_t rpm) = 0;
};
