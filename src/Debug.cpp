#include "Debug.hpp"

#include "Arduino.h"

void printArr(const uint8_t *arr, uint8_t size)
{
    for (int i = 0; i < size; i++)
        Serial.printf("%X ", arr[i]);
    Serial.println();
}