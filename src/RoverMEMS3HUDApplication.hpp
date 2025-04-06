#pragma once

#include "HUD.hpp"
#include "MEMS3Communications.hpp"

class RoverMEMS3HUDApplication
{
private:
    HUD *hud;
    MEMS3Communications *communicator;

    uint8_t speed = 0;
    uint16_t rpm = 0;

    bool inInitializationSequence = true;

public:
    RoverMEMS3HUDApplication();
    void loop();
};
