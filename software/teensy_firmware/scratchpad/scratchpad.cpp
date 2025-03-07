#include <Arduino.h>

bool truth_table(uint8_t multiplexer_channel, uint8_t control)
{
    uint8_t mask = 1 << control;
    return (multiplexer_channel & mask) != 0;
}

void setup()
{
}

void loop()
{
    for (uint8_t multiplexer_channel = 0; multiplexer_channel < 16; multiplexer_channel++)
    {
        Serial.print(multiplexer_channel);
        Serial.print(": ");
        Serial.print(truth_table(multiplexer_channel, 0));
        Serial.print(truth_table(multiplexer_channel, 1));
        Serial.print(truth_table(multiplexer_channel, 2));
        Serial.print(truth_table(multiplexer_channel, 3));
        Serial.println();
    }
    delay(10000);
}
