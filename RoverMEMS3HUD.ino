#include <U8g2lib.h>
#include "src/Debug.hpp"
#include "src/MEMS3Commands.hpp"
#include "src/SH1107HUD.hpp"
#include "src/MEMS3CommunicationsSerial.h"

SH1107HUD *hud;
MEMS3Communications *commandSender;

uint8_t speed = 0;
uint16_t rpm = 0;

void setup()
{
    hud = new SH1107HUD();
    commandSender = new MEMS3CommunicationsSerial();

#if DEBUG
    Serial.begin(115200);
    Serial.println("Starting Rover MEMS3 HUD");
#endif

    delay(1000); // let some time to make sure everything is initialized
}

#if SHOW_FRAMERATE
uint64_t prevTime = 0;
#endif

void loop()
{
    if (!commandSender->init_command_sequence_ack)
    {
        commandSender->runInitializationCommandSequence();
        delay(1000); // ensure initialization finished
    }
    
#if SHOW_FRAMERATE
    prevTime = millis();
#endif

    speed = commandSender->getSpeed();
    rpm = commandSender->getRpm();

    hud->draw(speed, rpm);

#if SHOW_FRAMERATE
    Serial.printf("%ldms\n", 1000 / (millis() - prevTime));
#endif
}
