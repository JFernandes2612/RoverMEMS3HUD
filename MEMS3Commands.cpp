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

bool ackCommand(const std::vector<uint8_t> &received, const std::vector<uint8_t> &expected)
{
    uint8_t minSize = DATA_START_INDEX + 1 + expected.size(); // The +1 is because of the checksum
    if (received.size() < minSize)
        return false;
    std::vector<uint8_t> responseCode = std::vector<uint8_t>(received.begin() + DATA_START_INDEX, received.begin() + DATA_START_INDEX + expected.size());
    return responseCode == expected;
}

const std::vector<uint8_t> getDataFromResponse(const std::vector<uint8_t> &response)
{
    if (response.size() <= ACTUAL_DATA_START_INDEX + 1) // The +1 is because of the checksum
        return std::vector<uint8_t>();
    return std::vector<uint8_t>(response.begin() + ACTUAL_DATA_START_INDEX, *(response.begin() + DATA_SIZE_INDEX) + response.begin() + DATA_START_INDEX);
}

int32_t bit(int32_t val, int32_t num_bit)
{
    return (val >> num_bit) & 1;
}

const uint16_t getWordFromResponseData(const std::vector<uint8_t> &data, const uint8_t wordStart)
{
  if (response.size() <= ACTUAL_DATA_START_INDEX + 3 + wordStart) // The +3 is because of the checksum
        return 0;
  return (((uint16_t)data[wordStart+ACTUAL_DATA_START_INDEX]) << 8) + (uint16_t)data[wordStart+ACTUAL_DATA_START_INDEX+1];
}

const uint8_t getByteFromResponseData(const std::vector<uint8_t> &data, const uint8_t byteNumber)
{
  if (response.size() <= ACTUAL_DATA_START_INDEX + 2 + byteNumber) // The +2 is because of the checksum
        return 0;
  return data[byteNumber+ACTUAL_DATA_START_INDEX];
}

const std::vector<uint8_t> generateKey(const std::vector<uint8_t> &seed)
{
    int32_t seedValue = (((int32_t)seed[0]) << 8) + (int32_t)seed[1];
    int32_t keyValue = 0;
    int32_t loops = 1;

    if (bit(seedValue, 15))
        loops += 8;
    if (bit(seedValue, 7))
        loops += 4;
    if (bit(seedValue, 4))
        loops += 2;
    if (bit(seedValue, 0))
        loops += 1;

    while (loops > 0)
    {
        keyValue = seedValue >> 1;

        if (bit(seedValue, 13) && bit(seedValue, 3))
            keyValue &= 0b11111111111111110;
        else
            keyValue |= 0b0000000000000001;

        int32_t v = bit(seedValue, 9) ^ bit(seedValue, 8) ^ bit(seedValue, 2) ^ bit(seedValue, 1);
        if (v)
            keyValue |= 0b1000000000000000;

        seedValue = keyValue;
        loops--;
    }

    std::vector<uint8_t> key;
    keyValue &= 0xFFFF;
    key.push_back((uint8_t)(keyValue >> 8));
    key.push_back((uint8_t)(keyValue & 0xFF));
    return key;
}
