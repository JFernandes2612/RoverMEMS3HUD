#include "RoverMEMS3HUDApplication.hpp"

#include "SH1107HUD.hpp"
#include "MEMS3CommunicationsSerial.hpp"

RoverMEMS3HUDApplication::RoverMEMS3HUDApplication()
{
    this->hud = new SH1107HUD();
    this->communicator = new MEMS3CommunicationsSerial();

    this->hud->draw(this->speed, this->rpm);

    this->communicator->runInitializationCommandSequence();
}

void RoverMEMS3HUDApplication::loop()
{
    this->speed = this->communicator->getSpeed();
    this->rpm = this->communicator->getRpm();

    this->hud->draw(this->speed, this->rpm);
}