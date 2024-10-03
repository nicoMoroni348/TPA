#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_Sensor.h>
#include <ThingSpeak.h>
#include "DHT22.h"
#include "Potenciometro.h"
#include "Display.h"
#include "LEDController.h"
#include "WifiManager.h"

// Definición de constantes y variables globales
const char *WIFI_SSID = "Wokwi-GUEST"; // AndroidAP2992?
const char *WIFI_PASSWORD = "";        // q1w2e3r4t5?
const char *BOT_TOKEN = "7968370248:AAGJW2-cDjW_71rJGP-Z--uKA58N8ccysX4";
const unsigned long BOT_MTBS = 1000; // Tiempo entre escaneos de mensajes (en milisegundos)
unsigned long channelID = 2670807;
const char *WriteAPIKey = "5O5DX1BTSAM7TLS5";

// Definicion de pineado de la placa
#define PIN_LED23 23     // Pin donde está conectado el LED
#define PIN_LED2 2       // Pin donde está conectado el LED
#define PIN_POT 32       // Pin donde está conectado el potenciómetro
#define SENSOR 33        // Pin donde está conectado el sensor DHT22
#define I2C_ADDRESS 0x3C // Dirección I2C del display

DHT22Sensor sensor(SENSOR);           // Instancia del sensor DHT22
Potenciometro potenciometro(PIN_POT); // Instancia del potenciómetro
Display display;                      // Instancia del display OLED
LEDController led23(PIN_LED23);       // Instancia del controlador del LED 23 (verde)
LEDController led2(PIN_LED2);         // Instancia del controlador del LED 2 (azul)
WifiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);

WiFiClient client;
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long tiempo_ultimo_mensaje; // Tiempo del último escaneo de mensajes
bool Start = false;
String tiempo_milis;

// Funciones declaradas
void conectarWifi();
void manejarMensajesTelegram(int numMensajesNuevos);
void procesarComandosTelegram(const String &chat_id, const String &mensaje, const String &nombre_usuario);
String obtenerTiempoFormateado(unsigned long millis);
void mostrarLog(const String &mensaje);

void manejarMensajesTelegram(int numMensajesNuevos)
{
  for (int i = 0; i < numMensajesNuevos; i++)
  {
    // Obtiene los datos del mensaje
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Guest";

    procesarComandosTelegram(chat_id, text, from_name); // Procesa los comandos recibidos
  }
}

void setup()
{
  Serial.begin(115200); // Inicializa el monitor serial

  conectarWifi();                     // Conecta a la red Wi-Fi
  wifiManager.sincronizarTiempoNTP(); // Sincroniza la hora con NTP
  display.inicializarDisplay();       // Inicializa el display OLED

  ThingSpeak.begin(client); // Inicializa la comunicación con ThingSpeak
}

void loop()
{
  display.actualizarDisplay("Esperando mensajes...", 0);

  if (millis() - tiempo_ultimo_mensaje > BOT_MTBS)
  {                                                                        // Comprueba si ha pasado el tiempo de escaneo
    int numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1); // Obtiene los mensajes nuevos

    while (numMensajesNuevos)
    {                                                                    // Mientras haya mensajes nuevos
      manejarMensajesTelegram(numMensajesNuevos);                        // Procesa los mensajes nuevos
      numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1); // Actualiza el contador de mensajes
    }
    tiempo_ultimo_mensaje = millis();
  }
  delay(1000);
}

void procesarComandosTelegram(const String &chat_id, const String &mensaje, const String &nombre_usuario)
{
  if (mensaje != "") // Si el mensaje no está vacío y no es válido
  {
    if (mensaje == "/start")
    {
      Start = true; // Habilitar todos los comandos
      String welcomeMessage = "¡Hola, " + nombre_usuario + "! 🎉\n\n";
      welcomeMessage += "Qué bueno verte por acá. A continuación te dejo los comandos que podés usar:\n\n";
      welcomeMessage += "💡🟢 /led23<on/off> - Controla el LED Verde\n\n";
      welcomeMessage += "💡🔵 /led2<on/off> - Controla el LED Azul\n\n";
      welcomeMessage += "🌡️ /dht22 - Obtén lecturas del sensor DHT22\n\n";
      welcomeMessage += "🎚️ /pote - Lee el valor potenciómetro\n\n";
      welcomeMessage += "📡 /platiot - Envía datos a IoT\n\n";
      welcomeMessage += "🖥️ /display<component> - Muestra el estado del componente\n\n";
      welcomeMessage += "¡¿Listo para comenzar?! ¡Elegí el comando que vos quiera!";
      bot.sendMessage(chat_id, welcomeMessage, "");
      mostrarLog("Se ha iniciado la conversación con el usuario " + nombre_usuario);
    }
    if (!Start)
    {
      bot.sendMessage(chat_id, "⚠️ Para empezar, por favor inicializá con el comando /start para acceder a todas las funcionalidades disponibles🚀", "");
    }
    else
    {
      if (mensaje == "/led23on")
      {
        led23.encenderLed();
        mostrarLog("LED 23 encendido");
        bot.sendMessage(chat_id, "💡🟢 Led 23 encendido.", "");
      }
      else if (mensaje == "/led23off")
      {
        led23.apagarLed();
        mostrarLog("LED 23 apagado");
        bot.sendMessage(chat_id, "💡🟢 Led 23 apagado", "");
      }
      else if (mensaje == "/led2on")
      {
        led2.encenderLed();
        mostrarLog("LED 2 encendido");
        bot.sendMessage(chat_id, "💡🔵 Led 2 encendido", "");
      }
      else if (mensaje == "/led2off")
      {
        led2.apagarLed();
        mostrarLog("LED 2 apagado");
        bot.sendMessage(chat_id, "💡🔵 Led 2 apagado", "");
      }
      else if (mensaje == "/dht22")
      {
        mostrarLog("Leyendo sensor DHT22...");
        float humedad = sensor.leerHumedad();
        float temperatura = sensor.leerTemperatura();
        String respuesta = "🌡️Humedad: " + String(humedad) + "%\n";
        respuesta += "🌡️Temperatura: " + String(temperatura) + "°C";
        bot.sendMessage(chat_id, respuesta);
      }
      else if (mensaje == "/pote")
      {
        float voltaje = potenciometro.leerVoltaje(3.3); // Lee el valor del potenciómetro
        String mensaje_pote = "El valor del potenciómetro es: " + String(voltaje) + "V";
        mostrarLog(mensaje_pote);
        bot.sendMessage(chat_id, "🎚️" + mensaje_pote, ""); // Envía el valor del potenciómetro al chat
      }
      else if (mensaje == "/platiot")
      {
        // enviar datos a IoT
        mostrarLog("Enviando datos a ThingSpeak...");
        bot.sendMessage(chat_id, "📡Enviando datos a ThingSpeak...", "");

        float humedad = sensor.leerHumedad();
        float temperatura = sensor.leerTemperatura();

        ThingSpeak.setField(1, temperatura);
        ThingSpeak.setField(2, humedad);

        int responseCode = ThingSpeak.writeFields(channelID, WriteAPIKey);
        if (responseCode == 200)
        {
          mostrarLog("Datos enviados a ThingSpeak!");
          bot.sendMessage(chat_id, "✅ Datos enviados a ThingSpeak!\n✅ Puedes visualizarlo en https://thingspeak.com/channels/2670807", "");
        }
        else
        {
          mostrarLog("Error al enviar datos: ");
          bot.sendMessage(chat_id, "❌ Error al enviar datos a ThingSpeak!", "");
          Serial.println("ERROR | " + String(responseCode));
        }
      }
      else if (mensaje.startsWith("/display"))
      {
        if (mensaje == "/displayled23")
        {
          String estado_led23 = led23.leerEstado();
          mostrarLog("Mostrando estado del LED 23 en OLED...");
          bot.sendMessage(chat_id, "🖥️ Mostrando estado del LED 23 en OLED...", "");
          display.actualizarDisplay("LED 23: " + estado_led23, 5000);
        }
        else if (mensaje == "/displayled2")
        {
          String estado_led2 = led2.leerEstado();
          mostrarLog("Mostrando estado del LED 2 en OLED...");
          bot.sendMessage(chat_id, "🖥️ Mostrando estado del LED 2 en OLED...", "");
          display.actualizarDisplay("LED 2: " + estado_led2, 5000);
        }
        else if (mensaje == "/displaypote")
        {
          float voltaje = potenciometro.leerVoltaje(3.3);
          String mensaje_pote = "🎚️ Potenciometro: " + String(voltaje) + "V";
          mostrarLog("Mostrando estado del potenciómetro en OLED...");
          bot.sendMessage(chat_id, "🖥️ Mostrando estado del potenciómetro en OLED...", "");
          display.actualizarDisplay("Potenciometro: " + String(voltaje) + "V", 5000);
        }
        else if (mensaje == "/displaysensor")
        {
          float humedad = sensor.leerHumedad();
          float temperatura = sensor.leerTemperatura();
          String mensaje_sensor = "🌡️ Humedad: " + String(humedad) + "%\n";
          mensaje_sensor += "🌡️ Temperatura: " + String(temperatura) + " C";
          display.actualizarDisplay("Humedad: " + String(humedad) + "%", 5000);

          mostrarLog("Mostrando estado del sensor DHT22 en OLED...");
          bot.sendMessage(chat_id, "🖥️ Mostrando estado del sensor DHT22 en OLED...", "");
        }
        else
        {
          String mensaje_error = "Display Invalido";
          bot.sendMessage(chat_id, mensaje_error, "");
          display.actualizarDisplay(mensaje_error, 5000);
        }
      } // Fin de la sección de comandos de display
      else
      {
        String mensaje_error = "Comando Invalido";
        bot.sendMessage(chat_id, mensaje_error, "");
        display.actualizarDisplay(mensaje_error, 5000);
      }
    }
  }
}

// Función para conectarse a la red Wi-Fi
void conectarWifi()
{
  Serial.print("Conectando a la red Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

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

// Imprimir logs en el monitor serial
String obtenerTiempoFormateado(unsigned long millis)
{
  unsigned long seconds = millis / 1000;
  unsigned long minutes = (seconds / 60) % 60;

  return String(minutes) + "m " + String(seconds % 60) + "s | INFO | ";
}

void mostrarLog(const String &mensaje)
{
  tiempo_milis = obtenerTiempoFormateado(millis());
  Serial.println(tiempo_milis + mensaje);
}