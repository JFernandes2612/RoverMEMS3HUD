#include "MEMS3Commands.hpp"

#include <cstdlib>
#include <cstring>

const std::vector<uint8_t> createCommand(const std::vector<uint8_t> &command)
{
    std::vector<uint8_t> finalCommand;
    finalCommand += MEMS3_HEADER;
    finalCommand.push_back(command.size());
    finalCommand += command;
    finalCommand.push_back(xorAll(finalCommand));

    return finalCommand;
}

const std::vector<uint8_t> createCommandWithData(const std::vector<uint8_t> &command, const std::vector<uint8_t> &data)
{
    std::vector<uint8_t> commandWithData = std::vector(command);
    return createCommand(commandWithData += data);
}

enum AckResult ackCommand(const std::vector<uint8_t> &received, const std::vector<uint8_t> &expected)
{
    if (received.size() < DATA_START_INDEX + 1 + expected.size())
        return AckResult::INVALID;
    if (received[DATA_START_INDEX] == NACK_COMMAND)
        return AckResult::NACK;
    std::vector<uint8_t> responseCode = std::vector<uint8_t>(received.begin() + DATA_START_INDEX, received.begin() + DATA_START_INDEX + expected.size());
    return responseCode == expected ? AckResult::ACK : AckResult::INVALID;
}

uint16_t bit(uint16_t val, uint16_t num_bit)
{
    return (val >> num_bit) & 1;
}

uint16_t getWordFromResponse(const std::vector<uint8_t> &data, const uint8_t wordStart)
{
    if (data.size() <= ACTUAL_DATA_START_INDEX + 2 + wordStart) // The +2 is because of the checksum + 1 byte of buffer to make up for the word
        return 0xFFFF;
    return (((uint16_t)data[wordStart + ACTUAL_DATA_START_INDEX]) << 8) + (uint16_t)data[wordStart + ACTUAL_DATA_START_INDEX + 1];
}

uint8_t getByteFromResponse(const std::vector<uint8_t> &data, const uint8_t byteNumber)
{
    if (data.size() <= ACTUAL_DATA_START_INDEX + 1 + byteNumber) // The +1 is because of the checksum
        return 0xFF;
    return data[byteNumber + ACTUAL_DATA_START_INDEX];
}

const std::vector<uint8_t> generateKey(uint16_t seedValue)
{
    uint16_t keyValue = 0;
    uint16_t loops = 1;

    if (bit(seedValue, 15))
        loops += 8;
    if (bit(seedValue, 7))
        loops += 4;
    if (bit(seedValue, 4))
        loops += 2;
    if (bit(seedValue, 0))
        loops++;

    for (uint16_t _ = 0; _ < loops; _++)
    {
        keyValue = (((seedValue >> 1) & 0xFFFE) | (bit(seedValue, 13) && bit(seedValue, 3) ? 0 : 1)) & 0x7FFF;

        if (bit(seedValue, 9) ^ bit(seedValue, 8) ^ bit(seedValue, 2) ^ bit(seedValue, 1))
            keyValue |= 0x8000;

        seedValue = keyValue;
    }

    std::vector<uint8_t> key;
    key.push_back((uint8_t)(keyValue >> 8));
    key.push_back((uint8_t)(keyValue & 0xFF));
    return key;
}
