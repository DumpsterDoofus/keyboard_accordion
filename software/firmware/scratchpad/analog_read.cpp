#include <Arduino.h>
#include <time.h>

const int pin_0 = A2;

void disable(uint8_t pin)
{
    pinMode(pin, INPUT_DISABLE);
}

void setup()
{
    disable(pin_0);
    analogReadAveraging(32);
}


void loop()
{
    auto elapsed = millis();
    auto value = 0;
    for (auto i = 0; i < 1000; i++)
    {
        value = analogRead(pin_0);
        Serial.println(value);
    }
    elapsed = millis() - elapsed;

    // Serial.print("Elapsed: ");
    // Serial.print(elapsed);
    // Serial.print(", value: ");
    // Serial.println(value);

    // delay(1000);
}
