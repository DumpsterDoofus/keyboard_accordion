#include <ADC.h>
#include <ADC_util.h>
#include <ADC_Module.h>

#include "basic_sensor.h"
#include "max_velocity_sensor.h"
#include "mean_velocity_sensor.h"
#include "pressure_sensor.h"

// This detects whether the PCB is powered by the external AC adapter.
const int dc_power_input = 35;

// These read the multiplexers (AM1 through AM12).
const int multiplexer_inputs[12] = {
    23,
    22,
    21,
    20,
    19,
    18,
    17,
    16,
    15,
    14,
    41,
    40,
};

// These control the multiplexers (CONTROL1 through CONTROL4).
const int multiplexer_outputs[4] = {
    39,
    38,
    37,
    36,
};

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

    // Uses B system (Eastern European) key layout.
    UseBSystemLayout = 5,

    // Uses C system (Western European) key layout.
    UseCSystemLayout = 6,

    // Resets settings to default values.
    FactoryReset = 7,
};

Config config;
MeanVelocitySensor velocity_sensor{config};
PressureSensor pressure_sensor{config};
BasicSensor basic_sensor{config};

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

void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
{
    if (config.calibrating)
    {
        config.handle_sensor_reading(index, sensor_reading);
        return;
    }

    switch (config.playing_mode)
    {
    case PlayingMode::VelocitySensing:
        velocity_sensor.handle_sensor_reading(index, sensor_reading);
        return;

    case PlayingMode::PressureSensing:
        pressure_sensor.handle_sensor_reading(index, sensor_reading);
        return;

    case PlayingMode::BasicSensing:
        basic_sensor.handle_sensor_reading(index, sensor_reading);
        return;

    default:
        Serial.print("Unexpected mode, this is a bug: ");
        Serial.println(static_cast<int>(config.playing_mode));
        return;
    }
}

void setup()
{
    // Disabling digital input for multiplexer analog inputs because "pinMode" docs suggests doing this to minimize energy use.
    for (auto multiplexer_input : multiplexer_inputs)
    {
        pinMode(multiplexer_input, INPUT_DISABLE);
    }
    // TODO: Disable unused pins?

    for (auto multiplexer_output : multiplexer_outputs)
    {
        pinMode(multiplexer_output, OUTPUT);
    }

    pinMode(dc_power_input, INPUT);

    setup_adc(*adc.adc0);
    setup_adc(*adc.adc1);

    config.load();
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
            config.begin_calibrating();
            break;

        case Command::CompleteCalibrating:
            config.complete_calibrating();
            break;

        case Command::CancelCalibrating:
            config.cancel_calibrating();
            break;

        case Command::EnableVelocitySensing:
            config.set_playing_mode(PlayingMode::VelocitySensing);
            break;

        case Command::EnablePressureSensing:
            config.set_playing_mode(PlayingMode::PressureSensing);
            break;

        case Command::UseBSystemLayout:
            config.use_b_system_layout();
            break;

        case Command::UseCSystemLayout:
            config.use_c_system_layout();
            break;

        case Command::FactoryReset:
            config.save_default_config();
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

    auto elapsed = micros();
    for (uint8_t multiplexer_channel = 0; multiplexer_channel < 16; multiplexer_channel++)
    {
        for (uint8_t multiplexer_output = 0; multiplexer_output < 4; multiplexer_output++)
        {
            digitalWrite(multiplexer_outputs[multiplexer_output], truth_table(multiplexer_channel, multiplexer_output));
        }

        for (uint8_t multiplexer_input = 0; multiplexer_input < 12; multiplexer_input += 2)
        {
            auto sensor_reading = adc.analogSynchronizedRead(
                multiplexer_inputs[multiplexer_input + 0],
                multiplexer_inputs[multiplexer_input + 1]);

            auto index0 = (multiplexer_input + 0) * 16 + multiplexer_channel;
            auto index1 = (multiplexer_input + 1) * 16 + multiplexer_channel;
            handle_sensor_reading(index0, sensor_reading.result_adc0);
            handle_sensor_reading(index1, sensor_reading.result_adc1);
        }
    }
    elapsed = micros() - elapsed;

#ifdef LOG_LATENCY
    Serial.print("Elapsed microseconds: ");
    Serial.println(elapsed);
#endif

#ifdef LOG_SENSOR_READINGS
    Serial.print("Sensor readings: ");
    switch (config.playing_mode)
    {
    case PlayingMode::VelocitySensing:
        print_array(velocity_sensor.sensor_readings);
        break;

    case PlayingMode::PressureSensing:
        print_array(pressure_sensor.sensor_readings);
        break;

    case PlayingMode::BasicSensing:
        print_array(basic_sensor.sensor_readings);
        break;

    default:
        Serial.print("Unexpected mode, this is a bug: ");
        Serial.println(static_cast<int>(config.playing_mode));
        return;
    }
#endif

    print_errors(*adc.adc0);
    print_errors(*adc.adc1);
}
