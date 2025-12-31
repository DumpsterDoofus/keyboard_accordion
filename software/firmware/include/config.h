#pragma once

#include <SD.h>

#include "util.h"

enum class PlayingMode
{
    // Playing in velocity-sensitive mode (like a piano). Most MIDI instruments support this.
    VelocitySensing = 0,

    // Playing in polytonic-aftertouch mode (press distance of each key controls its volume). Very few MIDI instruments support this.
    PressureSensing = 1,

    // Playing in basic on/off mode (like a traditional organ). Most MIDI instruments support this.
    BasicSensing = 2,
};

char* to_string(PlayingMode playing_mode)
{
    switch (playing_mode)
    {
    case PlayingMode::VelocitySensing:
        return "Velocity Sensing";

    case PlayingMode::PressureSensing:
        return "Pressure Sensing";

    case PlayingMode::BasicSensing:
        return "Basic Sensing";

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
    uint8_t index_to_midi_note[192];

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

    void use_b_system_layout()
    {
        Serial.println("Switching to B system key layout.");
        int tmp[] = {
            // Each MIDI note from 19-109 appears twice. Notes down to 14 or up to 114 occur 0-2 times.
            42, 39, 36, 33, 30, 27, 24, 21, 19, 22, 25, 28, 31, 34, 37, 40, 38, 35, 32, 29, 26, 23, 20, 17, 18, 21, 24, 27, 30, 33, 36, 39, 37, 34, 31, 28, 25, 22, 19, 16, 14, 17, 20, 23, 26, 29, 32, 35, 61, 58, 55, 52, 49, 46, 43, 40, 38, 41, 44, 47, 50, 53, 56, 59, 62, 59, 56, 53, 50, 47, 44, 41, 42, 45, 48, 51, 54, 57, 60, 63, 66, 63, 60, 57, 54, 51, 48, 45, 43, 46, 49, 52, 55, 58, 61, 64, 90, 87, 84, 81, 78, 75, 72, 69, 67, 70, 73, 76, 79, 82, 85, 88, 86, 83, 80, 77, 74, 71, 68, 65, 66, 69, 72, 75, 78, 81, 84, 87, 85, 82, 79, 76, 73, 70, 67, 64, 62, 65, 68, 71, 74, 77, 80, 83, 109, 106, 103, 100, 97, 94, 91, 88, 86, 89, 92, 95, 98, 101, 104, 107, 110, 107, 104, 101, 98, 95, 92, 89, 90, 93, 96, 99, 102, 105, 108, 111, 114, 111, 108, 105, 102, 99, 96, 93, 91, 94, 97, 100, 103, 106, 109, 112
        };
        std::copy(std::begin(tmp), std::end(tmp), index_to_midi_note);
        save();
    }

    void use_c_system_layout()
    {
        Serial.println("Switching to C system key layout.");
        int tmp[] = {
            // Each MIDI note from 15-110 appears twice.
            38, 35, 32, 29, 26, 23, 20, 17, 16, 19, 22, 25, 28, 31, 34, 37, 36, 33, 30, 27, 24, 21, 18, 15, 17, 20, 23, 26, 29, 32, 35, 38, 37, 34, 31, 28, 25, 22, 19, 16, 15, 18, 21, 24, 27, 30, 33, 36, 61, 58, 55, 52, 49, 46, 43, 40, 39, 42, 45, 48, 51, 54, 57, 60, 60, 57, 54, 51, 48, 45, 42, 39, 41, 44, 47, 50, 53, 56, 59, 62, 62, 59, 56, 53, 50, 47, 44, 41, 40, 43, 46, 49, 52, 55, 58, 61, 86, 83, 80, 77, 74, 71, 68, 65, 64, 67, 70, 73, 76, 79, 82, 85, 84, 81, 78, 75, 72, 69, 66, 63, 65, 68, 71, 74, 77, 80, 83, 86, 85, 82, 79, 76, 73, 70, 67, 64, 63, 66, 69, 72, 75, 78, 81, 84, 109, 106, 103, 100, 97, 94, 91, 88, 87, 90, 93, 96, 99, 102, 105, 108, 108, 105, 102, 99, 96, 93, 90, 87, 89, 92, 95, 98, 101, 104, 107, 110, 110, 107, 104, 101, 98, 95, 92, 89, 88, 91, 94, 97, 100, 103, 106, 109
        };
        std::copy(std::begin(tmp), std::end(tmp), index_to_midi_note);
        save();
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

        Serial.print("Index to MIDI note mapping: ");
        print_array(index_to_midi_note);
    }

    void save_default_config()
    {
        Serial.println("Saving default configuration.");
        playing_mode = PlayingMode::BasicSensing;

        fill_array(calibration_result.min_sensor_readings, std::numeric_limits<int32_t>::max());
        fill_array(calibration_result.max_sensor_readings, std::numeric_limits<int32_t>::min());

        use_c_system_layout();

        calibrating = false;

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

