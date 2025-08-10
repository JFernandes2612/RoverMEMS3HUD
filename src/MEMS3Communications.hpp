#pragma once

#include <vector>
#include <cstdint>

class MEMS3Communications
{
private:
    virtual void sendCommand(const std::vector<uint8_t> &command) = 0;
    virtual std::vector<uint8_t> readResponse(const uint8_t commandSize) = 0;
    std::vector<uint8_t> tryCommand(const std::vector<uint8_t> &command, const std::vector<uint8_t> &commandAck, uint32_t max_reties, const uint16_t retryDelayMs);

public:
    void runInitializationCommandSequence();
    uint8_t getSpeed();
    uint16_t getRpm();
};
