#include <Arduino.h>

void setup()
{
}

void loop()
{
    if (Serial.available())
    {
        auto incomingByte = Serial.read();
        Serial.print("Received: ");
        Serial.println(incomingByte);
    }
}
