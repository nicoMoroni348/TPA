#ifndef TELEGRAM_BOT_H
#define TELEGRAM_BOT_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "LEDController.h"
#include "DHT22.h"
#include "Potenciometro.h"
#include "Display.h"
#include <ThingSpeak.h>


class TelegramBot {
public:
    TelegramBot(const String& botToken, WiFiClientSecure& securedClient, 
                LEDController& led23, LEDController& led2, 
                DHT22Sensor& sensor, Potenciometro& potenciometro, Display& display);
    
    void manejarMensajesTelegram();
    void procesarComandosTelegram(const String& chat_id, const String& mensaje, const String& nombre_usuario);
    void enviarLogSerial(const String& mensaje);
    void iniciarBot();
    
private:
    String obtenerTiempoFormateado(unsigned long millis);
    void manejarComandoStart(const String& chat_id, const String& nombre_usuario);
    void manejarComandoLed(const String& chat_id, const String& mensaje);
    void manejarComandoDHT22(const String& chat_id);
    void manejarComandoPote(const String& chat_id);
    void manejarComandoDisplay(const String& chat_id, const String& mensaje);
    void manejarComandoPlatiot(const String& chat_id);
    void descartarMensajesPendientes();
    
    UniversalTelegramBot bot;
    unsigned long tiempo_ultimo_mensaje;
    bool Start;
    
    LEDController& led23;
    LEDController& led2;
    DHT22Sensor& sensor;
    Potenciometro& potenciometro;
    Display& display;
};

#endif
