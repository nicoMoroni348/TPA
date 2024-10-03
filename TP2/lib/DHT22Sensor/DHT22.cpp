#include "DHT22.h"

DHT22Sensor::DHT22Sensor(int pin) : dht(pin, DHT22)
{
    dht.begin();
}

float DHT22Sensor::leerTemperatura()
{
    return dht.readTemperature();
}

float DHT22Sensor::leerHumedad()
{
    return dht.readHumidity();
}
