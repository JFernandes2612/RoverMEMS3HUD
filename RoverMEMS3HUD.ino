#include <U8g2lib.h>
#include "src/MEMS3Commands.hpp"
#include "src/SH1107HUD.hpp"
#include "src/MEMS3CommunicationsSerial.h"

#define RX_PIN 16
#define TX_PIN 17
#define EN_PIN 4
#define COMMAND_RESPONSE_BUFF_SIZE 64

SH1107HUD* hud;
MEMS3Communications* commandSender;

uint8_t speed = 0;
uint16_t rpm = 0;

void setup()
{
    commandSender = new MEMS3CommunicationsSerial();
    hud = new SH1107HUD();

    hud->begin();

    Serial.begin(115200);
    Serial.println("Starting Rover MEMS3 HUD");

    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, HIGH);

    Serial2.begin(9600, SERIAL_8E1, RX_PIN, TX_PIN);
    Serial2.setTimeout(10);

    delay(1000); // let some time to make sure everything is initialized
}

uint64_t prevTime = 0;

void loop()
{
    if (!commandSender->init_command_sequence_ack)
    {
        commandSender->runInitializationCommandSequence();
        delay(1000); // ensure initialization finished
    }
    prevTime = millis();

    speed = commandSender->getSpeed();
    rpm = commandSender->getRpm();

    hud->draw(speed, rpm);

    Serial.printf("%ldms\n", 1000 / (millis() - prevTime));
}
