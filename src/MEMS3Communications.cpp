#include "MEMS3Communications.hpp"

#include "Arduino.h"

#include "Debug.hpp"
#include "MEMS3Commands.hpp"

std::vector<uint8_t> MEMS3Communications::tryCommand(const std::vector<uint8_t> &command, const std::vector<uint8_t> &commandAck, const uint16_t retryDelayMs = 5000)
{
    std::vector<uint8_t> response;
    while (1)
    {
        this->sendCommand(command);
        response = this->readResponse(command.size());
#if DEBUG
        if (response.empty())
            Serial.printf(" Retrying in %dms\n", retryDelayMs);
        else
        {
            enum AckResult ack = ackCommand(response, commandAck);
            if (ack == AckResult::ACK)
                break;
            else if (ack == AckResult::NACK)
            {
                Serial.println("Nack");
                return {};
            }
            else
                Serial.printf("Ack Failed. Retrying in %dms\n", retryDelayMs);
        }
#else
        if (!response.empty())
        {
            enum AckResult ack = ackCommand(response, commandAck);
            if (ack == AckResult::ACK)
                break;
            else if (ack == AckResult::NACK)
                return {};
        }
#endif
        delay(retryDelayMs);
    }
    return response;
}

void MEMS3Communications::runInitializationCommandSequence()
{
    while (true)
    {
        if (this->tryCommand(createCommand(MEMS3_START_DIAGNOSTIC_COMMAND), MEMS3_START_DIAGNOSTIC_COMMAND_ACK).empty())
            continue;

        std::vector<uint8_t> seed = this->tryCommand(createCommand(MEMS3_REQUEST_SEED_COMMAND), MEMS3_REQUEST_SEED_COMMAND_ACK);
        if (seed.empty())
            continue;

        uint16_t seedValue = getWordFromResponse(seed, 0);
        if (seedValue == 0)
            return;

        if (!this->tryCommand(createCommandWithData(MEMS3_SEND_KEY_COMMAND, generateKey(seedValue)), MEMS3_SEND_KEY_COMMAND_ACK).empty())
            break;
    }
}

uint8_t MEMS3Communications::getSpeed()
{
    return getByteFromResponse(this->tryCommand(createCommand(MEMS3_ID_0D_COMMAND), MEMS3_ID_0D_COMMAND_ACK, 0), 0);
}

uint16_t MEMS3Communications::getRpm()
{
    return getWordFromResponse(this->tryCommand(createCommand(MEMS3_ID_06_COMMAND), MEMS3_ID_06_COMMAND_ACK, 0), 8);
}