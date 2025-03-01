#include "Arduino.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "MEMS3Commands.hpp"

#define RX_PIN 16
#define TX_PIN 17
#define EN_PIN 4
#define COMMAND_RESPONSE_BUFF_SIZE 64

bool init_command_sequence_ack = false;

U8G2_SH1107_128X128_F_HW_I2C u8g2(U8G2_R0);

const uint8_t CENTER_X = 64;
const uint8_t CENTER_Y = 64;

const uint8_t FONT_X = 29;
const uint8_t FONT_Y = 100;

const uint8_t MAX_SPEED = 200;
uint8_t speed = 0;

const uint8_t RPM_ARC_RADIUS = 53;
const uint8_t RPM_ARC_START = 20;
const uint8_t RPM_ARC_END = 108;
const uint8_t RPM_ARC_THICKNESS = 3;

const uint16_t MAX_RPM = 8000;
uint16_t rpm = 0;

void setup()
{
    u8g2.begin();
    u8g2.setContrast(255);

    // 1sh draw - Draw all 0 screen
    draw();

    Serial.begin(115200);

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

void draw_speed()
{
    u8g2.setFont(u8g2_font_fub30_tn);
    u8g2.drawStr(FONT_X, FONT_Y, u8x8_u8toa(speed, 3));

    u8g2.setFont(u8g2_font_resoledbold_tr);
    u8g2.drawStr(85, 110, "km/h");
}

void draw_rpm()
{
    u8g2.drawArc(CENTER_X, CENTER_Y, RPM_ARC_RADIUS, map(rpm, 0, MAX_RPM, RPM_ARC_END - 1, RPM_ARC_START), RPM_ARC_END, RPM_ARC_THICKNESS);
    u8g2.setFont(u8g2_font_fub11_tr);
    u8g2.drawStr(CENTER_X - 18, CENTER_Y - 25, u8x8_u16toa(rpm, 4));
    u8g2.setFont(u8g2_font_resoledbold_tr);
    u8g2.drawStr(CENTER_X + 22, CENTER_Y - 18, "rpm");
}

void draw()
{
    u8g2.clearBuffer();
    draw_speed();
    draw_rpm();
    u8g2.sendBuffer();
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

    draw();

    Serial.printf("%ldms\n", millis() - prevTime);
}
