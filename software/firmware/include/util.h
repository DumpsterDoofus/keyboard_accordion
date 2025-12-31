#pragma once

#include <Arduino.h>

// Logs total time used to read all the keys.
#define LOG_LATENCY

// Logs each individual key press.
#define LOG_KEY_PRESSES

// Logs all keys in CSV format.
#define LOG_SENSOR_READINGS

template<typename T, size_t N>
void print_array(const T (&array)[N])
{
    for (auto element : array)
    {
        Serial.print(element);
        Serial.print(',');
    }
    Serial.println();
}

void fill_array(int32_t (&array)[192], int32_t value)
{
    std::fill(std::begin(array), std::end(array), value);
}
