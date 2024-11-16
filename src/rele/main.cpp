#include <Arduino.h>

#define pin 33

void setup()
{
    pinMode(pin, OUTPUT);
    pinMode(2, OUTPUT);
}
void loop()
{
    digitalWrite(pin, HIGH);
    digitalWrite(2, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
    digitalWrite(2, LOW);
    delay(1000);
}