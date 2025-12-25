#pragma once
#include "config.cpp"

struct MaxVelocitySensor
{
    int32_t sensor_readings[192];
    float_t max_velocities[192];
    Config& config;

    MaxVelocitySensor(Config& config) : config(config)
    {
    }

    void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
    {
        auto note = config.index_to_midi_note[index];

        auto last_sensor_reading = sensor_readings[index];
        sensor_readings[index] = sensor_reading;

        // 0 is fully unpressed, 1 is fully pressed.
        auto displacement = (sensor_reading - config.calibration_result.min_sensor_readings[index]) / static_cast<float>(config.calibration_result.max_sensor_readings[index] - config.calibration_result.min_sensor_readings[index]);

        auto velocity = sensor_reading - last_sensor_reading;
        auto max_velocity = max_velocities[index];

        // Max velocity encodes 3 states:
        // == 0 means key is "not yet pressed"
        // > 0 means key is "being pressed"
        // == -1 means key is "being released"
        if (max_velocity >= 0)
        {
            // Transition into "being pressed" when key is at least 10% pressed, to ignore noise in the unpressed state.
            if (displacement > 0.1f)
            {
                if (velocity > max_velocity)
                {
                    max_velocity = velocity;
                    max_velocities[index] = velocity;
                }
            }
            // The "actuation point" is when the key is at least 90% pressed or it begins moving backwards while in "being pressed".
            if (displacement > 0.9f || velocity <= 0 && max_velocity > 0)
            {
                // Transition to "being released".
                max_velocities[index] = -1.0f;

                // TODO: Need to tune this based off actual velocity from testing.
                const float velocity_multiplier = 1.0f;

                // MIDI velocity range is 0 to 127. Anything higher than 127 will alias down into the 0-127 range. See https://arduinomidilib.sourceforge.net/a00001.html.
                auto midi_velocity = static_cast<uint8_t>(std::clamp(static_cast<int>(std::round(velocity_multiplier * displacement * max_velocity)), 0, 127));
                usbMIDI.sendNoteOn(note, midi_velocity, 0);

                #ifdef LOG_KEY_PRESSES
                Serial.print("Sent note ");
                Serial.print(note);
                Serial.print(" on with velocity ");
                Serial.print(midi_velocity);
                Serial.println('.');
                #endif
            }
        }
        // Key is "being released".
        else
        {
            if (displacement < 0.1f)
            {
                // Transition to "not yet pressed".
                max_velocities[index] = 0.0f;

                usbMIDI.sendNoteOff(note, 0, 0);

                #ifdef LOG_KEY_PRESSES
                Serial.print("Sent note ");
                Serial.print(note);
                Serial.println(" off.");
                #endif
            }
        }
    }

};
