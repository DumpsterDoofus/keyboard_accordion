#include <ADC.h>
#include <ADC_util.h>
#include <ADC_Module.h>

const int pin_0 = A2;
const int pin_1 = A1;

ADC& adc = *new ADC();

void setup_adc(ADC_Module& adc_module)
{
    adc_module.setAveraging(32);
    adc_module.setResolution(10);
    adc_module.setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc_module.setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);
}

void disable(uint8_t pin)
{
    pinMode(pin, INPUT_DISABLE);
}

void print_if_errored(ADC_Module& adc_module)
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

void setup()
{
    disable(pin_0);
    disable(pin_1);

    setup_adc(*adc.adc0);
    setup_adc(*adc.adc1);
}

void loop()
{
    auto elapsed = millis();
    ADC::Sync_result result;
    for (size_t i = 0; i < 96; i++)
    {
        result = adc.analogSynchronizedRead(pin_0, pin_1);
        // print(result);
    }
    elapsed = millis() - elapsed;

    Serial.print("Elapsed: ");
    Serial.print(elapsed);
    Serial.print(", value: [");
    Serial.print(result.result_adc0);
    Serial.print(", ");
    Serial.print(result.result_adc1);
    Serial.println("]");

    print_if_errored(*adc.adc0);
    print_if_errored(*adc.adc1);

    delay(1000);
}
