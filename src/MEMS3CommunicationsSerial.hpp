#pragma once

#include "MEMS3Communications.hpp"

#define COMMAND_RESPONSE_BUFF_SIZE 64

class MEMS3CommunicationsSerial : public MEMS3Communications
{
public:
    MEMS3CommunicationsSerial();

private:
    void sendCommand(const std::vector<uint8_t> &command);
    std::vector<uint8_t> readResponse(const uint8_t commandSize);
};
