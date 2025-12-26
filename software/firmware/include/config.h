#pragma once

#include <SD.h>

#include "util.h"

enum class PlayingMode
{
    // Playing in velocity-sensitive mode (like a piano). Virtually all MIDI instruments support this.
    VelocitySensing = 0,

    // Playing in polytonic-aftertouch mode (press distance of each key controls its volume). Very few MIDI instruments support this.
    PressureSensing = 1,
};

char* to_string(PlayingMode playing_mode)
{
    switch (playing_mode)
    {
    case PlayingMode::VelocitySensing:
        return "Velocity Sensing";

    case PlayingMode::PressureSensing:
        return "Pressure Sensing";

    default:
        return "Unknown";
    }
}

// The result of a calibration procedure, where each key is pressed fully down and up.
struct CalibrationResult
{
    int32_t min_sensor_readings[192];
    int32_t max_sensor_readings[192];
};

// User settings. This is persisted to the SD card so that it isn't lost during a power cycle.
struct Config
{
    PlayingMode playing_mode;
    CalibrationResult calibration_result;
    uint8_t index_to_midi_note[192] = {
        // TODO: Figure this out once hardware arrives. Add B vs C system support.
    };

    // If the sensors are being "factory calibrated".
    bool calibrating;

    void begin_calibrating()
    {
        if (calibrating)
        {
            Serial.println("Received command to begin calibration, but calibration is already in progress, so doing nothing.");
        }
        else
        {
            fill_array(calibration_result.min_sensor_readings, std::numeric_limits<int32_t>::max());
            fill_array(calibration_result.max_sensor_readings, std::numeric_limits<int32_t>::min());
            calibrating = true;
            Serial.println("Beginning calibration.");
        }
    }

    void complete_calibrating()
    {
        if (calibrating)
        {
            calibrating = false;
            Serial.println("Saving calibration results.");
            save();
        }
        else
        {
            Serial.println("Received command to complete calibration, but calibration was never begun, so doing nothing.");
        }
    }

    void cancel_calibrating()
    {
        if (calibrating)
        {
            calibrating = false;
            Serial.println("Discarding calibration results.");
            load();
        }
        else
        {
            Serial.println("Received command to cancel calibration, but calibration was never begun, so doing nothing.");
        }
    }

    void set_playing_mode(PlayingMode playing_mode)
    {
        if (this->playing_mode == playing_mode)
        {
            Serial.println("Received request to change playing mode, but the requested playing mode is already the current playing mode, so doing nothing.");
        }
        else
        {
            Serial.println("Saving update to playing mode.");
            this->playing_mode = playing_mode;
            save();
        }
    }

    void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
    {
        if (sensor_reading < calibration_result.min_sensor_readings[index])
        {
            calibration_result.min_sensor_readings[index] = sensor_reading;
        }

        if (sensor_reading > calibration_result.max_sensor_readings[index])
        {
            calibration_result.max_sensor_readings[index] = sensor_reading;
        }
    }

    void print()
    {
        Serial.print("Playing mode: ");
        Serial.println(to_string(playing_mode));

        Serial.print("Min sensor readings: ");
        print_array(calibration_result.min_sensor_readings);

        Serial.print("Max sensor readings: ");
        print_array(calibration_result.max_sensor_readings);
    }

    void save_default_config()
    {
        playing_mode = PlayingMode::VelocitySensing;

        fill_array(calibration_result.min_sensor_readings, 300);
        fill_array(calibration_result.max_sensor_readings, 480);

        save();
    }

    void load()
    {
        while (true)
        {
            if (!SD.begin(BUILTIN_SDCARD))
            {
                Serial.println("SD card was not found, but is required to store settings. Please insert an SD card.");
                delay(1000);
                continue;
            }

            if (!SD.exists("dugmetara.config"))
            {
                Serial.println("Previous settings were not found on SD card, so creating default settings.");
                save_default_config();
                return;
            }

            auto config_file = SD.open("dugmetara.config", FILE_READ);
            if (!config_file)
            {
                config_file.close();
                Serial.println("Failed to open previous settings on SD card. Retrying.");
                delay(1000);
                continue;
            }

            auto expected_bytes = sizeof(Config);
            auto actual_bytes = config_file.read(reinterpret_cast<char *>(this), expected_bytes);
            config_file.close();

            if (expected_bytes != actual_bytes)
            {
                Serial.print("Previous SD card settings were found, but not enough data was present. Expected bytes: ");
                Serial.print(expected_bytes);
                Serial.print(", actual bytes: ");
                Serial.print(actual_bytes);
                Serial.println(". Creating default settings.");

                save_default_config();
                return;
            }
            else
            {
                Serial.println("Successfully loaded settings from SD card.");
                print();
                return;
            }
        }
    }

    void save()
    {
        while (true)
        {
            if (!SD.begin(BUILTIN_SDCARD))
            {
                Serial.println("SD card was not found, but is required to store settings. Please insert an SD card.");
                delay(1000);
                continue;
            }

            auto config_file = SD.open("dugmetara.config", FILE_WRITE_BEGIN);
            if (!config_file)
            {
                config_file.close();
                Serial.println("Failed to open previous settings on SD card. Retrying.");
                delay(1000);
                continue;
            }

            auto expected_bytes = sizeof(Config);
            auto actual_bytes = config_file.write(reinterpret_cast<const char *>(this), expected_bytes);
            config_file.close();

            if (expected_bytes != actual_bytes)
            {
                Serial.print("Failed to write settings to SD card. Expected bytes written: ");
                Serial.print(expected_bytes);
                Serial.print(", actual bytes written: ");
                Serial.print(actual_bytes);
                Serial.println(". Retrying.");
                continue;
            }
            else
            {
                Serial.println("Successfully saved settings to SD card.");
                print();
                return;
            }
        }
    }
};

