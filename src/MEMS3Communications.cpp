#include "MEMS3Communications.hpp"
#include "Arduino.h"
#include "Debug.hpp"
#include "MEMS3Commands.hpp"

std::vector<uint8_t> MEMS3Communications::tryCommand(const std::vector<uint8_t> &command, const std::vector<uint8_t> &commandAck, const uint16_t retryDelayMs = 1000)
{
    std::vector<uint8_t> response;
    while (1)
    {
        this->sendCommand(command);
        response = this->readResponse(command.size());
#if DEBUG
        if (response.empty())
            Serial.printf(" Retrying in %d milliseconds\n", retryDelayMs);
        else
        {
            if (ackCommand(response, commandAck))
                break;
            else
                Serial.printf("\nAck Failed. Retrying in %d milliseconds\n", retryDelayMs);
        }
#else
        if (!response.empty() && ackCommand(response, commandAck))
            break;
#endif
        delay(retryDelayMs);
    }
    return response;
}

void MEMS3Communications::runInitializationCommandSequence()
{
    this->tryCommand(createCommand(MEMS3_INIT_COMMAND), MEMS3_INIT_COMMAND_ACK);

    this->tryCommand(createCommand(MEMS3_START_DIAGNOSTIC_COMMAND), MEMS3_START_DIAGNOSTIC_COMMAND_ACK);

    uint16_t seedValue = getWordFromResponse(this->tryCommand(createCommand(MEMS3_REQUEST_SEED_COMMAND), MEMS3_REQUEST_SEED_COMMAND_ACK), 0);
    if (seedValue == 0)
    {
        this->init_command_sequence_ack = true;
        return;
    }

    this->tryCommand(createCommandWithData(MEMS3_SEND_KEY_COMMAND, generateKey(seedValue)), MEMS3_SEND_KEY_COMMAND_ACK);

    this->init_command_sequence_ack = true;
}

uint8_t MEMS3Communications::getSpeed()
{
    return getByteFromResponse(this->tryCommand(createCommand(MEMS3_ID_0D_COMMAND), MEMS3_ID_0D_COMMAND_ACK, 0), 0);
}

uint16_t MEMS3Communications::getRpm()
{
    return getWordFromResponse(this->tryCommand(createCommand(MEMS3_ID_06_COMMAND), MEMS3_ID_06_COMMAND_ACK, 0), 8);
}