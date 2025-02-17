#include <Arduino.h>

void on(int milliseconds)
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(milliseconds);
}

void off(int milliseconds)
{
    digitalWrite(LED_BUILTIN, LOW);
    delay(milliseconds);
}

void blink(int milliseconds, int times)
{
    for (size_t i = 0; i < times; i++)
    {
        on(milliseconds);
        off(milliseconds);
    }
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    blink(1000, 1);
    blink(100, 10);
    blink(25, 40);
    off(10000);
}
