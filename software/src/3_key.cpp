#include <ADC.h>
#include <ADC_util.h>
#include <ADC_Module.h>

// Logs total time used to read all the keys.
#define LOG_LATENCY

// Logs each individual key press.
#define LOG_KEY_PRESSES

// Logs all keys in CSV format.
#define LOG_SENSOR_READINGS

// Enables polytonic aftertouch mode (depression of each key controls its volume). Note most MIDI instruments don't support this. If unset, instead uses velocity sensitivity (like a piano) which virtually all MIDI instruments support.
#define USE_POLYPHONIC_AFTERTOUCH

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


ADC& adc = *new ADC();

void setup_adc(ADC_Module& adc_module)
{
    adc_module.setAveraging(32);
    adc_module.setResolution(10);
    adc_module.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc_module.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
}

/// @brief Errors will probably only occur if the ADC has been tasked with reading a PIN that it's not connected to, which would be a bug.
/// @param adc_module 
void print_errors(ADC_Module& adc_module)
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

void handle_sensor_reading(uint8_t index, int32_t sensor_reading)
{
    #ifdef USE_POLYPHONIC_AFTERTOUCH
    handle_sensor_reading_pressure(index, sensor_reading);
    #else
    handle_sensor_reading_velocity(index, sensor_reading);
    #endif
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
}

void loop()
{
    if (!digitalRead(dc_power_input))
    {
        // This was recommended at https://forum.pjrc.com/index.php?threads/beginner-schematic-review-hall-effect-keyboard.76407/#post-354443.
        Serial.println("The board doesn't appear to have power. Setting all outputs to LOW to avoid driving voltage into unpowered multiplexers.");
        digitalWrite(multiplexer_output_0, LOW);
        digitalWrite(multiplexer_output_1, LOW);
        digitalWrite(multiplexer_output_2, LOW);
        digitalWrite(multiplexer_output_3, LOW);

        delay(1000);
        return;
    }

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
    for (auto sensor_reading : sensor_readings)
    {
        Serial.print(sensor_reading);
        Serial.print(',');
    }
    Serial.println();
    #endif

    print_errors(*adc.adc0);
    print_errors(*adc.adc1);

    // This won't be present in the 192-key board. Only doing this in the 3-key board to limit sample rate to a manageable level.
    delay(5);
}
