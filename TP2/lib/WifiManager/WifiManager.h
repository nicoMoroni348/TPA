#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>

class WifiManager
{
public:
    WifiManager(const char *ssid, const char *password);
    void conectarWifi();
    void sincronizarTiempoNTP();

private:
    const char *ssid;
    const char *password;
};

#endif