#include <U8g2lib.h>
#include "src/MEMS3Commands.hpp"
#include "src/SH1107HUD.hpp"

#define RX_PIN 16
#define TX_PIN 17
#define EN_PIN 4
#define COMMAND_RESPONSE_BUFF_SIZE 64

bool init_command_sequence_ack = false;

SH1107HUD hud;

uint8_t speed = 0;
uint16_t rpm = 0;

void setup()
{
    hud.begin();

    Serial.begin(115200);
    Serial.println("Starting Rover MEMS3 HUD");

    pinMode(EN_PIN, OUTPUT);
    digitalWrite(EN_PIN, HIGH);

    Serial2.begin(9600, SERIAL_8E1, RX_PIN, TX_PIN);
    Serial2.setTimeout(10);

    delay(1000); // let some time to make sure everything is initialized
}

void printArr(const uint8_t *arr, uint8_t size)
{
    for (int i = 0; i < size; i++)
        Serial.printf("%X ", arr[i]);
    Serial.println();
}

void sendCommand(const std::vector<uint8_t> &command)
{
    Serial.print("Sending data: ");
    printArr(&command[0], command.size());
    Serial2.write(&command[0], command.size());
}

std::vector<uint8_t> readResponse(uint8_t commandSize)
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

std::vector<uint8_t> tryCommand(const std::vector<uint8_t> &command, const std::vector<uint8_t> &commandAck, const uint16_t retryDelayMs = 1000)
{
    std::vector<uint8_t> response;
    while (1)
    {
        sendCommand(command);
        response = readResponse(command.size());
        if (response.empty())
            Serial.printf(" Retrying in %d milliseconds\n", retryDelayMs);
        else
        {
            if (ackCommand(response, commandAck))
                break;
            else
                Serial.printf("\nAck Failed. Retrying in %d milliseconds\n", retryDelayMs);
        }
        delay(retryDelayMs);
    }
    return response;
}

void runInitializationCommandSequence()
{
    tryCommand(createCommand(MEMS3_INIT_COMMAND), MEMS3_INIT_COMMAND_ACK);

    tryCommand(createCommand(MEMS3_START_DIAGNOSTIC_COMMAND), MEMS3_START_DIAGNOSTIC_COMMAND_ACK);

    uint16_t seedValue = getWordFromResponse(tryCommand(createCommand(MEMS3_REQUEST_SEED_COMMAND), MEMS3_REQUEST_SEED_COMMAND_ACK), 0);
    if (seedValue == 0)
    {
        init_command_sequence_ack = true;
        return;
    }

    tryCommand(createCommandWithData(MEMS3_SEND_KEY_COMMAND, generateKey(seedValue)), MEMS3_SEND_KEY_COMMAND_ACK);

    init_command_sequence_ack = true;
}

uint64_t prevTime = 0;

void loop()
{
    if (!init_command_sequence_ack)
    {
        runInitializationCommandSequence();
        delay(1000); // ensure initialization
    }
    prevTime = millis();

    speed = getByteFromResponse(tryCommand(createCommand(MEMS3_ID_0D_COMMAND), MEMS3_ID_0D_COMMAND_ACK, 0), 0);
    rpm = getWordFromResponse(tryCommand(createCommand(MEMS3_ID_06_COMMAND), MEMS3_ID_06_COMMAND_ACK, 0), 8);

    hud.draw(speed, rpm);

    Serial.printf("%ldms\n", millis() - prevTime);
}
