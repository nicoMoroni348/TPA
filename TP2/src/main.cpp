#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "LEDController.h"
#include "DHT22.h"
#include "Potenciometro.h"
#include "Display.h"
#include "TelegramBot.h"
#include "WifiManager.h"

// Configuración de Wi-Fi
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// Configuración de Telegram
const String BOT_TOKEN = "7968370248:AAGJW2-cDjW_71rJGP-Z--uKA58N8ccysX4";

// Pines
#define LED_PIN23 23
#define LED_PIN2 2
#define DHT_PIN 33
#define POT_PIN 32

// Instancias de objetos
WiFiClientSecure secured_client;
LEDController led23(LED_PIN23);
LEDController led2(LED_PIN2);
DHT22Sensor sensor(DHT_PIN);
Potenciometro potenciometro(POT_PIN);
Display display;
TelegramBot bot(BOT_TOKEN, secured_client, led23, led2, sensor, potenciometro, display);
WifiManager wifiManager(ssid, password);
WiFiClient client;

void setup()
{
  Serial.begin(115200);

  // Conectar a Wi-Fi
  Serial.print("Conectando a la red Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a la red Wi-Fi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  // Conexión segura
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  //
  ThingSpeak.begin(client);

  wifiManager.sincronizarTiempoNTP(); // Sincroniza la hora con NTP

  // Inicializar los componentes de hardware
  display.inicializarDisplay();

  // Inicia el bot
  Serial.println("Iniciando bot...");
  bot.iniciarBot();
  Serial.println("Bot iniciado");

  // Mostrar mensaje inicial en el display
  display.actualizarDisplay("Bot Iniciado", 10);
}

void loop()
{
  // Manejar mensajes entrantes de Telegram
  bot.manejarMensajesTelegram();
}