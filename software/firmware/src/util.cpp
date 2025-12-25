#pragma once
#include <Arduino.h>

void print_array(int32_t (&array)[192])
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
