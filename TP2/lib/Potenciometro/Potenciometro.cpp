#include "Potenciometro.h"

Potenciometro::Potenciometro(int pin) : pin(pin)
{
    pinMode(pin, INPUT);
}

int Potenciometro::leerValor()
{
    return analogRead(pin);
}

float Potenciometro::leerVoltaje(float vMax)
{
    // (valor_analogico / 4095.0) * 3.3
    return (analogRead(pin) / 4095.0) * vMax;
}
