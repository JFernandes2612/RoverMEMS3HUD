#include "RoverMEMS3HUDApplication.hpp"

#include "SH1107HUD.hpp"
#include "MEMS3CommunicationsSerial.hpp"

RoverMEMS3HUDApplication::RoverMEMS3HUDApplication()
{
    this->hud = new SH1107HUD();
    this->communicator = new MEMS3CommunicationsSerial();

    this->hud->draw(this->speed, this->rpm);
}

void RoverMEMS3HUDApplication::loop()
{
    if (this->inInitializationSequence)
    {
        this->communicator->runInitializationCommandSequence();
        this->inInitializationSequence = false;
    }

    this->speed = this->communicator->getSpeed();
    this->rpm = this->communicator->getRpm();

    if (speed == 0xFF || rpm == 0xFFFF)
    {
        this->inInitializationSequence = true;
        this->hud->draw(0, 0);
        return;
    }

    this->hud->draw(this->speed, this->rpm);
}