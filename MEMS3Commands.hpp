#pragma once
#include <cstdint>
#include <vector>
#include <numeric>

#define DATA_START_INDEX 4
#define DATA_SIZE_INDEX 3
#define ACTUAL_DATA_START_INDEX 6

const static std::vector<uint8_t> MEMS3_HEADER = {0xB8, 0x13, 0xF7};

const static std::vector<uint8_t> MEMS3_INIT_COMMAND = {0x1A, 0x9A};
const static std::vector<uint8_t> MEMS3_INIT_COMMAND_ACK = {0x5A, 0x9A};

const static std::vector<uint8_t> MEMS3_START_DIAGNOSTIC_COMMAND = {0x10, 0xA0};
const static std::vector<uint8_t> MEMS3_START_DIAGNOSTIC_COMMAND_ACK = {0x50};

const static std::vector<uint8_t> MEMS3_REQUEST_SEED_COMMAND = {0x27, 0x01};
const static std::vector<uint8_t> MEMS3_REQUEST_SEED_COMMAND_ACK = {0x67, 0x01};

const static std::vector<uint8_t> MEMS3_SEND_KEY_COMMAND = {0x27, 0x02};
const static std::vector<uint8_t> MEMS3_SEND_KEY_COMMAND_ACK = {0x67, 0x02};

const static std::vector<uint8_t> MEMS3_ID_06_COMMAND = {0x21, 0x06};
const static std::vector<uint8_t> MEMS3_ID_06_COMMAND_ACK = {0x61, 0x06};

const static std::vector<uint8_t> MEMS3_ID_0D_COMMAND = {0x21, 0x0d};
const static std::vector<uint8_t> MEMS3_ID_0D_COMMAND_ACK = {0x61, 0x0d};

const std::vector<uint8_t> createCommand(const std::vector<uint8_t> &command);

const std::vector<uint8_t> createCommandWithData(const std::vector<uint8_t> &command, const std::vector<uint8_t> &data);

bool ackCommand(const std::vector<uint8_t> &received, const std::vector<uint8_t> &expected);

const std::vector<uint8_t> getDataFromResponse(const std::vector<uint8_t> &response);

const std::vector<uint8_t> generateKey(const std::vector<uint8_t> &seed);

const uint16_t getWordFromResponseData(const std::vector<uint8_t> &data, const uint8_t wordStart);

template <typename T>
std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T> &B)
{
    A.reserve(A.size() + B.size());
    A.insert(A.end(), B.begin(), B.end());
    return A;
}

template <typename T>
T xorAll(std::vector<T> v)
{
    return std::accumulate(v.begin(),
                           v.end(),
                           (T)0,
                           std::bit_xor<void>());
}

// const static uint8_t finalCommand[7] = {0xB8, 0x13, 0xF7, 0x02, 0x1A, 0x9A, 0xB8 ^ 0x13 ^ 0xF7 ^ 0x02 ^ 0x1A ^ 0x9A};
