#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <Arduino.h>

class LEDController
{ 
public:
    LEDController(int pin);
    void encenderLed();
    void apagarLed();
    String leerEstado();

private:
    int pin;
};

#endif
