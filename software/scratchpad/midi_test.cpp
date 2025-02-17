#include <Arduino.h>

void play(uint8_t note)
{
    Serial.print("Playing ");
    Serial.println(note);

    usbMIDI.sendNoteOn(note, 55, 0);
    usbMIDI.send_now();
    delay(100);

    usbMIDI.sendNoteOn(note, 55, 0);
    usbMIDI.send_now();
    delay(100);

    usbMIDI.sendNoteOn(note, 55, 0);
    usbMIDI.send_now();
    delay(100);

    usbMIDI.sendNoteOff(note, 55, 0);
    usbMIDI.send_now();
    delay(100);
}

void setup()
{
    usbMIDI.begin();
}

void loop()
{
    for (uint8_t note = 50; note < 56; note += 2)
    {
        play(note);
    }
}
