#pragma once

#include "config.h"

struct PressureSensor
{
    int32_t sensor_readings[192];
    Config& config;

    PressureSensor(Config& config) : config(config)
    {
    }

    uint8_t get_pressure(int32_t reading)
    {
        // TODO: Read from calibration
        const int min_reading = 300;
        const int max_reading = 480;
        auto pressure_float = 127.0f * (max_reading - reading) / (max_reading - min_reading);

        auto pressure_int = static_cast<int>(std::round(pressure_float));
        // MIDI pressure range is 0 to 127. Anything higher than 127 will alias down into the 0-127 range. See https://arduinomidilib.sourceforge.net/a00001.html.
        return static_cast<uint8_t>(std::clamp(pressure_int, 0, 127));
    }

    void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
    {
        auto note = config.index_to_midi_note[index];

        auto last_sensor_reading = sensor_readings[index];
        sensor_readings[index] = sensor_reading;

        auto pressure = get_pressure(sensor_reading);
        auto last_pressure = get_pressure(last_sensor_reading);

        if (pressure > 0)
        {
            if (last_pressure == 0)
            {
                usbMIDI.sendNoteOn(note, pressure, 0);

                #ifdef LOG_KEY_PRESSES
                Serial.print(note);
                Serial.print(" note on, velocity ");
                Serial.println(pressure);
                #endif
            }
            else
            {
                // LMMS > Triple Oscillator > HugeGrittyBass responds to PolyPressure messages.
                usbMIDI.sendPolyPressure(note, pressure, 0);

                // In practice, polyphonic pressure control of each individual key probably won't be useful. Instead will probably want to control all notes via a single foot pedal. I tried sendAfterTouch(), but it didn't seem to respond. As a workaround, doing this by connecting the volume knob in LMMS to the controller.
                // usbMIDI.sendControlChange(7, pressure, 0);

                #ifdef LOG_KEY_PRESSES
                Serial.print(note);
                Serial.print(" poly pressure ");
                Serial.println(pressure);
                #endif
            }
        }
        else
        {
            if (last_pressure > 0)
            {
                usbMIDI.sendNoteOff(note, 0, 0);

                #ifdef LOG_KEY_PRESSES
                Serial.print(note);
                Serial.println(" note off.");
                #endif
            }
        }
    }
};
