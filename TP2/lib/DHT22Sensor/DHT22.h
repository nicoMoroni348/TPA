#ifndef DHT22_H
#define DHT22_H

#include <DHT.h>

class DHT22Sensor
{
public:
    DHT22Sensor(int pin);
    float leerTemperatura();
    float leerHumedad();

private:
    DHT dht; // Esto debe ser inicializado en el constructor
};

#endif