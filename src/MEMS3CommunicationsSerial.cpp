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
    size_t commandResponseReadSize = Serial2.readBytes(commandResponseBuff, commandSize + DATA_START_INDEX);

    if (commandResponseReadSize == 0 || commandResponseReadSize < commandSize)
    {
#if DEBUG
        Serial.print("Could not read echo.");
#endif
        while (Serial2.available() > 0)
        {
            Serial2.read();
        }
        return std::vector<uint8_t>();
    }
#if DEBUG
    Serial.print("Echo: ");
    printArr(commandResponseBuff, commandSize);
#endif

    if (commandResponseReadSize < commandSize + DATA_START_INDEX)
    {
#if DEBUG
        Serial.print("Could not read data.");
#endif
        while (Serial2.available() > 0)
        {
            Serial2.read();
        }
        return std::vector<uint8_t>();
    }
    commandResponseReadSize = Serial2.readBytes(commandResponseBuff + commandResponseReadSize, commandResponseBuff[commandResponseReadSize - 1] + 1);
#if DEBUG
    Serial.print("Data: ");
    printArr(commandResponseBuff + commandSize, DATA_START_INDEX + commandResponseReadSize);
#endif

    return std::vector(commandResponseBuff + commandSize, commandResponseBuff + commandSize + DATA_START_INDEX + commandResponseReadSize);
}