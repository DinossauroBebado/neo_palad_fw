#include <Arduino.h>
#define ldr_pin 36

void setup()
{
    pinMode(ldr_pin, INPUT);
    Serial.begin(9600);
}
void loop()
{

    Serial.println(analogRead(ldr_pin));
}