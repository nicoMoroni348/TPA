#ifndef POTENCIOMETRO_H
#define POTENCIOMETRO_H

#include <Arduino.h>

class Potenciometro
{
public:
    Potenciometro(int pin);
    int leerValor();
    float leerVoltaje(float vMax = 5.0);

private:
    int pin;
};

#endif
