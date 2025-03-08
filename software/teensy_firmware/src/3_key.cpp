#include <algorithm>
#include <ADC.h>
#include <ADC_util.h>
#include <ADC_Module.h>
#include <limits>
#include <SD.h>
#include <string>
#include "wiring.h"

// Logs total time used to read all the keys.
#define LOG_LATENCY

// Logs each individual key press.
#define LOG_KEY_PRESSES

// Logs all keys in CSV format.
#define LOG_SENSOR_READINGS

// This detects whether the PCB is powered by the external AC adapter.
const int dc_power_input = 37;

// This pin is connected to multiplexer.
const int multiplexer_input_0 = A2;

// This pin is floating. Reading it anyways to validate parallel ADC readings work.
const int multiplexer_input_1 = A1;

const int multiplexer_output_0 = 36;
const int multiplexer_output_1 = 35;
const int multiplexer_output_2 = 34;
const int multiplexer_output_3 = 33;

// Although there's only 3 keys, going to write code to support the general case where both pins are connected to multiplexers (ie, 32 sensors).
int32_t sensor_readings[32];

// If the sensors are being "factory calibrated".
bool calibrating = false;

bool powered;

enum class Command
{
    // Begins the sensor calibration procedure.
    BeginCalibrating = 0,

    // Completes the sensor calibration procedure and saves the results.
    CompleteCalibrating = 1,

    // Cancels the sensor calibration procedure without saving the results.
    CancelCalibrating = 2,

    // Switches to velocity-sensitive mode.
    EnableVelocitySensing = 3,

    // Switches to polytonic-aftertouch mode.
    EnablePressureSensing = 4,
};

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
    int32_t min_sensor_readings[32];
    int32_t max_sensor_readings[32];
};

// User settings. This is persisted to the SD card so that it isn't lost during a power cycle.
struct Config
{
    PlayingMode playing_mode;
    CalibrationResult calibration_result;
};

Config config;

void print_array(int32_t (&array)[32])
{
    for (auto element : array)
    {
        Serial.print(element);
        Serial.print(',');
    }
    Serial.println();
}

void print_config()
{
    Serial.print("Playing mode: ");
    Serial.println(to_string(config.playing_mode));

    Serial.print("Min sensor readings: ");
    print_array(config.calibration_result.min_sensor_readings);

    Serial.print("Max sensor readings: ");
    print_array(config.calibration_result.max_sensor_readings);
}

void fill_array(int32_t (&array)[32], int32_t value)
{
    std::fill(std::begin(array), std::end(array), value);
}

void save_config()
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
        auto actual_bytes = config_file.write(reinterpret_cast<const char *>(&config), expected_bytes);
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
            print_config();
            return;
        }
    }
}

void save_default_config()
{
    config.playing_mode = PlayingMode::VelocitySensing;

    fill_array(config.calibration_result.min_sensor_readings, 300);
    fill_array(config.calibration_result.max_sensor_readings, 480);

    save_config();
}

void load_config()
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
        auto actual_bytes = config_file.read(reinterpret_cast<char *>(&config), expected_bytes);
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
            print_config();
            return;
        }
    }
}

void set_playing_mode(PlayingMode playing_mode)
{
    if (config.playing_mode == playing_mode)
    {
        Serial.println("Received request to change playing mode, but the requested playing mode is already the current playing mode, so doing nothing.");
    }
    else
    {
        Serial.println("Saving update to playing mode.");
        config.playing_mode = playing_mode;
        save_config();
    }
}

ADC &adc = *new ADC();

void setup_adc(ADC_Module &adc_module)
{
    adc_module.setAveraging(32);
    adc_module.setResolution(10);
    adc_module.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc_module.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
}

/// @brief Errors will probably only occur if the ADC has been tasked with reading a PIN that it's not connected to, which would be a bug.
/// @param adc_module
void print_errors(ADC_Module &adc_module)
{
    auto adc_error = adc_module.fail_flag;
    if (adc_error != ADC_ERROR::CLEAR)
    {
        Serial.print("ADC error: ");
        Serial.println(getStringADCError(adc_error));
    }
}

/// @brief This implements the truth table in https://www.lcsc.com/datasheet/lcsc_datasheet_1810010611_Texas-Instruments-CD74HC4067SM96_C98457.pdf.
/// @param multiplexer_channel From 0 to 15.
/// @param control From 0 to 3.
/// @return Whether the control should be set high.
bool truth_table(uint8_t multiplexer_channel, uint8_t control)
{
    uint8_t mask = 1 << control;
    return (multiplexer_channel & mask) != 0;
}

uint8_t get_velocity(int32_t reading_1, int32_t reading_2)
{
    const float velocity_multiplier = 2.0;

    auto velocity_float = velocity_multiplier * (reading_1 - reading_2);
    auto velocity_int = static_cast<int>(std::round(velocity_float));
    // MIDI velocity range is 0 to 127. Anything higher than 127 will alias down into the 0-127 range. See https://arduinomidilib.sourceforge.net/a00001.html.
    return static_cast<uint8_t>(std::clamp(velocity_int, 0, 127));
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

void handle_sensor_reading_velocity(uint8_t index, int32_t sensor_reading)
{
    const uint8_t note = 60;

    auto last_sensor_reading = sensor_readings[index];
    sensor_readings[index] = sensor_reading;

    if (index >= 4)
    {
        return;
    }

    // TODO: Get from calibration
    if (sensor_reading <= 350 && 350 < last_sensor_reading)
    {
        auto velocity = get_velocity(last_sensor_reading, sensor_reading);
        usbMIDI.sendNoteOn(note, velocity, 0);

#ifdef LOG_KEY_PRESSES
        Serial.print("Sent note on with velocity ");
        Serial.print(velocity);
        Serial.println('.');
#endif
    }
    if (last_sensor_reading <= 350 && 350 < sensor_reading)
    {
        usbMIDI.sendNoteOff(note, 0, 0);

#ifdef LOG_KEY_PRESSES
        Serial.println("Sent note off.");
#endif
    }
}

void handle_sensor_reading_pressure(uint8_t index, int32_t sensor_reading)
{
    const uint8_t note = 60;

    auto last_sensor_reading = sensor_readings[index];
    sensor_readings[index] = sensor_reading;

    if (index >= 3)
    {
        return;
    }

    auto pressure = get_pressure(sensor_reading);
    auto last_pressure = get_pressure(last_sensor_reading);

    if (pressure > 0)
    {
        if (last_pressure == 0)
        {
            usbMIDI.sendNoteOn(note, pressure, 0);

#ifdef LOG_KEY_PRESSES
            Serial.print(index);
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
            Serial.print(index);
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
            Serial.print(index);
            Serial.println(" note off.");
#endif
        }
    }
}

void handle_sensor_reading_calibration(uint8_t index, int32_t sensor_reading)
{
    if (sensor_reading < config.calibration_result.min_sensor_readings[index])
    {
        config.calibration_result.min_sensor_readings[index] = sensor_reading;
    }

    if (sensor_reading > config.calibration_result.max_sensor_readings[index])
    {
        config.calibration_result.max_sensor_readings[index] = sensor_reading;
    }
}

void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
{
    if (calibrating)
    {
        handle_sensor_reading_calibration(index, sensor_reading);
        return;
    }

    switch (config.playing_mode)
    {
    case PlayingMode::VelocitySensing:
        handle_sensor_reading_velocity(index, sensor_reading);
        return;

    case PlayingMode::PressureSensing:
        handle_sensor_reading_pressure(index, sensor_reading);
        return;

    default:
        // TODO: Should this raise an exception? What will the exception do to the Teensy?
        Serial.print("Unexpected mode, this is a bug: ");
        Serial.println(static_cast<int>(config.playing_mode));
        return;
    }
}

void begin_calibrating()
{
    if (calibrating)
    {
        Serial.println("Received command to begin calibration, but calibration is already in progress, so doing nothing.");
    }
    else
    {
        fill_array(config.calibration_result.min_sensor_readings, std::numeric_limits<int32_t>::max());
        fill_array(config.calibration_result.max_sensor_readings, std::numeric_limits<int32_t>::min());
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
        save_config();
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
        load_config();
    }
    else
    {
        Serial.println("Received command to cancel calibration, but calibration was never begun, so doing nothing.");
    }
}

void setup()
{
    // Disabling digital input for multiplexer analog inputs because "pinMode" docs suggests doing this to minimize energy use.
    pinMode(multiplexer_input_0, INPUT_DISABLE);
    pinMode(multiplexer_input_1, INPUT_DISABLE);
    // TODO: Disable unused pins?

    pinMode(multiplexer_output_0, OUTPUT);
    pinMode(multiplexer_output_1, OUTPUT);
    pinMode(multiplexer_output_2, OUTPUT);
    pinMode(multiplexer_output_3, OUTPUT);

    pinMode(dc_power_input, INPUT);

    setup_adc(*adc.adc0);
    setup_adc(*adc.adc1);

    load_config();
}

std::vector<int> commands = {};

void loop()
{
    if (Serial.available())
    {
        auto command = Serial.read();
        commands.push_back(command);
        switch (static_cast<Command>(command))
        {
        case Command::BeginCalibrating:
            begin_calibrating();
            break;

        case Command::CompleteCalibrating:
            complete_calibrating();
            break;

        case Command::CancelCalibrating:
            cancel_calibrating();
            break;

        case Command::EnableVelocitySensing:
            set_playing_mode(PlayingMode::VelocitySensing);
            break;

        case Command::EnablePressureSensing:
            set_playing_mode(PlayingMode::PressureSensing);
            break;

        default:
            // TODO: Should this raise an exception? What will the exception do to the Teensy?
            Serial.print("Unexpected command, this is a bug: ");
            Serial.println(command);
            break;
        }

        Serial.print("Commands received: ");
        for (auto command : commands)
        {
            Serial.print(command);
            Serial.print(',');
        }
        Serial.println();
    }

    // TODO: The below code is commented out because it doesn't seem to work. The intent was to read the external 3.3V to determine whether external power was connected, and disable/enable the multiplexers accordingly. However, in practice it appears that the first time the multiplexers are driven high, there is some kind of parasitic draw (?) that bleeds into the power plane that causes the voltage to read high, even after the AC adapter is disconnected.
    // if (!digitalRead(dc_power_input))
    // {
    //     Serial.println("The board has no power.");
    //     if (powered)
    //     {
    //         powered = false;

    //         // This was recommended at https://forum.pjrc.com/index.php?threads/beginner-schematic-review-hall-effect-keyboard.76407/#post-354443.
    //         Serial.println("The board doesn't appear to have power. Setting all outputs to LOW to avoid driving voltage into unpowered multiplexers.");
    //         digitalWrite(multiplexer_output_0, LOW);
    //         digitalWrite(multiplexer_output_1, LOW);
    //         digitalWrite(multiplexer_output_2, LOW);
    //         digitalWrite(multiplexer_output_3, LOW);
    //     }

    //     delay(1000);
    //     return;
    // }
    // else
    // {
    //     Serial.println("The board has power.");
    // }

    // powered = true;

    auto elapsed = millis();
    for (uint8_t multiplexer_channel = 0; multiplexer_channel < 16; multiplexer_channel++)
    {
        // Note: In the 192-key version, will probably want to flip order of operations to first read and second set control. Ie, read 1st pair of multiplexers, set 1st pair to next channel, ..., read 6th pair of multiplexers, set 6th pair to next channel, increment channel, and then repeat. This maximizes the amount of time between setting the channel and reading the channel, because 5 other readings will have taken place between them, which hopefully reduces noise. Not doing this here because there is only 1 pair, so flipping order does not increase the time between setting and reading.
        digitalWrite(multiplexer_output_0, truth_table(multiplexer_channel, 0));
        digitalWrite(multiplexer_output_1, truth_table(multiplexer_channel, 1));
        digitalWrite(multiplexer_output_2, truth_table(multiplexer_channel, 2));
        digitalWrite(multiplexer_output_3, truth_table(multiplexer_channel, 3));

        auto sensor_reading = adc.analogSynchronizedRead(
            multiplexer_input_0,
            multiplexer_input_1);

        handle_sensor_reading(multiplexer_channel, sensor_reading.result_adc0);
        handle_sensor_reading(multiplexer_channel + 16, sensor_reading.result_adc1);
    }
    elapsed = millis() - elapsed;

#ifdef LOG_LATENCY
    Serial.print("Elapsed milliseconds: ");
    Serial.println(elapsed);
#endif

#ifdef LOG_SENSOR_READINGS
    Serial.print("Sensor readings: ");
    print_array(sensor_readings);
#endif

    print_errors(*adc.adc0);
    print_errors(*adc.adc1);

    // This won't be present in the 192-key board. Only doing this in the 3-key board to limit sample rate to a manageable level.
    delay(5);
}
