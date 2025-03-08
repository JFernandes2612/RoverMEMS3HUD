#include "MEMS3CommunicationsSerial.hpp"
#include "Arduino.h"
#include "Debug.hpp"
#include "MEMS3Commands.hpp"

void MEMS3CommunicationsSerial::sendCommand(const std::vector<uint8_t> &command)
{
    Serial.print("Sending data: ");
    printArr(&command[0], command.size());
    Serial2.write(&command[0], command.size());
}

std::vector<uint8_t> MEMS3CommunicationsSerial::readResponse(const uint8_t commandSize)
{
    uint8_t commandResponseBuff[COMMAND_RESPONSE_BUFF_SIZE];
    size_t commandResponseReadSize = Serial2.readBytes(commandResponseBuff, commandSize + DATA_START_INDEX);

    if (commandResponseReadSize == 0 || commandResponseReadSize < commandSize)
    {
        Serial.print("Could not read echo.");
        while (Serial2.available() > 0)
        {
            Serial2.read();
        }
        return std::vector<uint8_t>();
    }
    Serial.print("Echo: ");
    printArr(commandResponseBuff, commandSize);

    if (commandResponseReadSize < commandSize + DATA_START_INDEX)
    {
        Serial.print("Could not read data.");
        while (Serial2.available() > 0)
        {
            Serial2.read();
        }
        return std::vector<uint8_t>();
    }
    commandResponseReadSize = Serial2.readBytes(commandResponseBuff + commandResponseReadSize, commandResponseBuff[commandResponseReadSize - 1] + 1);
    Serial.print("Data: ");
    printArr(commandResponseBuff + commandSize, DATA_START_INDEX + commandResponseReadSize);

    return std::vector(commandResponseBuff + commandSize, commandResponseBuff + commandSize + DATA_START_INDEX + commandResponseReadSize);
}