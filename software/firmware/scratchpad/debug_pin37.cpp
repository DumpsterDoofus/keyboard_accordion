#include <ADC.h>

const int dc_power_input = 37;
// This pin is connected to multiplexer.
const int multiplexer_input_0 = A2;

// This pin is floating. Reading it anyways to validate parallel ADC readings work.
const int multiplexer_input_1 = A1;

const int multiplexer_output_0 = 36;
const int multiplexer_output_1 = 35;
const int multiplexer_output_2 = 34;
const int multiplexer_output_3 = 33;

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
}

void loop()
{
    // digitalWrite(multiplexer_output_0, HIGH);
    // digitalWrite(multiplexer_output_1, HIGH);
    // digitalWrite(multiplexer_output_2, HIGH);
    // digitalWrite(multiplexer_output_3, HIGH);

    delay(500);

    digitalWrite(multiplexer_output_0, LOW);
    digitalWrite(multiplexer_output_1, LOW);
    digitalWrite(multiplexer_output_2, LOW);
    digitalWrite(multiplexer_output_3, LOW);

    Serial.println(analogRead(multiplexer_input_0));
}