#pragma once

#include "config.h"

struct BasicSensor
{
    int32_t sensor_readings[192];
    Config& config;

    BasicSensor(Config& config) : config(config)
    {
    }

    void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
    {
        auto note = config.index_to_midi_note[index];

        auto last_sensor_reading = sensor_readings[index];
        sensor_readings[index] = sensor_reading;

        if (last_sensor_reading >= 400 && sensor_reading < 400)
        {
            usbMIDI.sendNoteOn(note, 127, 0);

            #ifdef LOG_KEY_PRESSES
            Serial.print("Sent note on ");
            Serial.print(note);
            Serial.print(" for index ");
            Serial.print(index);
            Serial.print(" with velocity 127 because sensor reading dropped from ");
            Serial.print(last_sensor_reading);
            Serial.print(" to ");
            Serial.print(sensor_reading);
            Serial.println(".");
            #endif
        }
        else if (last_sensor_reading <= 400 && sensor_reading > 400)
        {
            usbMIDI.sendNoteOff(note, 0, 0);

            #ifdef LOG_KEY_PRESSES
            Serial.print("Sent note off ");
            Serial.print(note);
            Serial.print(" for index ");
            Serial.print(index);
            Serial.print(" because sensor reading rose from ");
            Serial.print(last_sensor_reading);
            Serial.print(" to ");
            Serial.print(sensor_reading);
            Serial.println(".");
            #endif
        }
    }

};
