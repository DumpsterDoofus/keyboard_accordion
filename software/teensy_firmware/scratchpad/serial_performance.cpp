#include <Arduino.h>

void setup()
{
}

// This logs as fast as possible, to benchmark serial logging performance.
void loop()
{
    for (auto i = 0; i < 192; i++)
    {
        Serial.print("1234567890123456789012345678901234567890");
    }
    // delay(1);
}
