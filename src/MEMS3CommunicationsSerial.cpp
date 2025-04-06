#include "MEMS3CommunicationsSerial.hpp"

#include "Arduino.h"

#include "Debug.hpp"
#include "MEMS3Commands.hpp"

#define RX_PIN 16
#define TX_PIN 17
#define EN_PIN 4

MEMS3CommunicationsSerial::MEMS3CommunicationsSerial()
{
    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, HIGH);

    Serial2.begin(9600, SERIAL_8E1, RX_PIN, TX_PIN);
    Serial2.setTimeout(10);
}

void MEMS3CommunicationsSerial::sendCommand(const std::vector<uint8_t> &command)
{
#if DEBUG
    Serial.print("Sending data: ");
    printArr(&command[0], command.size());
#endif
    Serial2.write(&command[0], command.size());
}

std::vector<uint8_t> MEMS3CommunicationsSerial::readResponse(const uint8_t commandSize)
{
    uint8_t commandResponseBuff[COMMAND_RESPONSE_BUFF_SIZE];
    size_t commandResponseReadSize = Serial2.readBytes(commandResponseBuff, commandSize);

    if (commandResponseReadSize == 0)
    {
#if DEBUG
        Serial.print("Could not read echo.");
#endif
        return std::vector<uint8_t>();
    }
#if DEBUG
    Serial.print("Echo: ");
    printArr(commandResponseBuff, commandSize);
#endif

    for (uint8_t i = 0; i < 2; i++)
    {
        commandResponseReadSize = Serial2.readBytes(commandResponseBuff, COMMAND_RESPONSE_BUFF_SIZE);

        if (commandResponseReadSize == 0)
        {
#if DEBUG
            Serial.print("Could not read data.");
#endif
            if (i)
                return std::vector<uint8_t>();
            else
                continue;
        }

#if DEBUG
        Serial.print("Data: ");
        printArr(commandResponseBuff, commandResponseReadSize);
#endif
        break;
    }

    return std::vector(commandResponseBuff, commandResponseBuff + commandResponseReadSize);
}
