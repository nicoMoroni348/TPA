#include "WifiManager.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>


WifiManager::WifiManager(const char *ssid, const char *password) : ssid(ssid), password(password) {}

void WifiManager::conectarWifi()
{
    Serial.print("Conectando a la red Wi-Fi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    // secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Conectado a la red Wi-Fi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

void WifiManager::sincronizarTiempoNTP()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    Serial.println("Sincronizando hora con NTP");
    time_t ahora = time(nullptr);

    while (ahora < 24 * 3600)
    {
        delay(500);
        Serial.print(".");
        ahora = time(nullptr);
    }
    Serial.println("");
    Serial.println("Hora sincronizada");
}
