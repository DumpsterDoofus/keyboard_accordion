#include <ADC.h>
#include <ADC_util.h>
#include <ADC_Module.h>

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

void print_errors(ADC_Module& adc_module)
{
    auto adc_error = adc_module.fail_flag;
    if (adc_error != ADC_ERROR::CLEAR)
    {
        Serial.print("ADC error: ");
        Serial.println(getStringADCError(adc_error));
    }
}


void print(ADC::Sync_result& result)
{
    Serial.print(result.result_adc0);
    Serial.print(',');
    Serial.print(result.result_adc1);
    Serial.println();
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

        sensor_readings[multiplexer_channel] = sensor_reading.result_adc0;
        sensor_readings[multiplexer_channel + 16] = sensor_reading.result_adc1;
    }
    elapsed = millis() - elapsed;

    Serial.print("Elapsed milliseconds: ");
    Serial.println(elapsed);

    Serial.print("Sensor readings: ");
    for (auto sensor_reading : sensor_readings)
    {
        Serial.print(sensor_reading);
        Serial.print(',');
    }
    Serial.println();

    print_errors(*adc.adc0);
    print_errors(*adc.adc1);

    delay(1000);
}
