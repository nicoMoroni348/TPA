#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_SH110X.h>


// Definición de constantes y variables globales
const char *WIFI_SSID = "Wokwi-GUEST"; // AndroidAP2992
const char *WIFI_PASSWORD = "";        // q1w2e3r4t5
const char *BOT_TOKEN = "7968370248:AAGJW2-cDjW_71rJGP-Z--uKA58N8ccysX4";
const unsigned long BOT_MTBS = 1000; // Tiempo entre escaneos de mensajes (en milisegundos)
const int PIN_LED23 = 23;
const int PIN_LED2 = 2;
const int PIN_POT = 32;  // Pin donde está conectado el potenciómetro
#define SENSOR 33
DHT sensor(SENSOR, DHT22);

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long tiempo_ultimo_mensaje; // Tiempo del último escaneo de mensajes
bool Start = false;

// Funciones declaradas
void conectarWifi();
void sincronizarTiempoNTP();
void manejarMensajesTelegram(int numMensajesNuevos);
void procesarComandosTelegram(const String &chat_id, const String &mensaje, const String &nombre_usuario);
void encenderLed(int pin);
void apagarLed(int pin);

void manejarMensajesTelegram(int numMensajesNuevos)
{
  for (int i = 0; i < numMensajesNuevos; i++)
  {
    // Obtiene los datos del mensaje
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    // Serial.println("Chat ID: " + chat_id);
    // Serial.println("Mensaje: " + text);
    // Serial.println("Nombre: " + from_name);

    if (from_name == "")
      from_name = "Guest";

    procesarComandosTelegram(chat_id, text, from_name); // Procesa los comandos recibidos
  }
}

void setup()
{
  Serial.begin(115200);       // Inicializa el monitor serial
  pinMode(PIN_LED23, OUTPUT); // Configura el pin del LED como salida
  pinMode(PIN_LED2, OUTPUT);  // Configura el pin del LED como salida

  conectarWifi();         // Conecta a la red Wi-Fi
  sincronizarTiempoNTP(); // Sincroniza la hora con NTP
}

void loop()
{
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

  if (mensaje == "/start")
  {
    Start = true; // Habilitar todos los comandos
    String welcomeMessage = "Bienvenido " + nombre_usuario + "!\n";
    welcomeMessage += "Ahora puedes utilizar los siguientes comandos:\n";
    welcomeMessage += "/led<GPIO><on/off> - Controlar LEDs\n";
    welcomeMessage += "/dht22 - Leer sensor DHT22\n";
    welcomeMessage += "/pote - Leer potenciómetro\n";
    welcomeMessage += "/platiot - Enviar datos a IoT\n";
    welcomeMessage += "/display<component> - Mostrar estado en OLED\n";
    bot.sendMessage(chat_id, welcomeMessage, "");
    Serial.println("Comandos disponibles enviados");
    Start = true;
  }
  if (!Start)
  {
    bot.sendMessage(chat_id, "Primero inicialice con el comando /start", "");
  }
  else
  {
    if (mensaje == "/led23on")
    {
      encenderLed(PIN_LED23);
      Serial.println("LED 23 encendido");
      bot.sendMessage(chat_id, "Led 23 encendido", "");
    }
    else if (mensaje == "/led23off")
    {
      apagarLed(PIN_LED23);
      Serial.println("LED 23 apagado");
      bot.sendMessage(chat_id, "Led 23 apagado", "");
    }
    else if (mensaje == "/led2on")
    {
      encenderLed(PIN_LED2);
      Serial.println("LED 2 encendido");
      bot.sendMessage(chat_id, "Led 2 encendido", "");
    }
    else if (mensaje == "/led2off")
    {
      apagarLed(PIN_LED2);
      Serial.println("LED 2 apagado");
      bot.sendMessage(chat_id, "Led 2 apagado", "");
    }
    else if (mensaje == "/dht22") //ESTE VA CON THINGSPEAK
    {
      // leer sensor DHT22
      Serial.println("Leyendo sensor DHT22...");
      bot.sendMessage(chat_id, "Leyendo sensor DHT22...", "");
      float humedad = sensor.readHumidity();
      float temperatura = sensor.readTemperature();
      String respuesta = "Humedad: " + String(humedad) + "%\n";
      respuesta += "Temperatura: " + String(temperatura) + "°C";
      bot.sendMessage(chat_id, respuesta);
    }
    else if (mensaje == "/pote") {
      // leer potenciómetro
      int valor_analogico = analogRead(PIN_POT);  // Lee el valor analógico del pin
      float voltaje = (valor_analogico / 4095.0) * 3.3;  // Convierte el valor a voltaje (0-3.3V)

      String mensaje_pote = "El valor del potenciómetro es: " + String(voltaje) + "V";
      
      Serial.println(mensaje_pote);
      bot.sendMessage(chat_id, mensaje_pote, "");  // Envía el valor del potenciómetro al chat
    }
    else if (mensaje == "/platiot")
    {
      // enviar datos a IoT
      Serial.println("Enviando datos a IoT...");
      bot.sendMessage(chat_id, "Enviando datos a IoT...", "");
    }
    else if (mensaje == "/displayled2")
    {
      // mostrar estado del LED 2 en OLED
      Serial.println("Mostrando estado del LED 2 en OLED...");
      bot.sendMessage(chat_id, "Mostrando estado del LED 2 en OLED...", "");
    }
    else if (mensaje == "/displayled23")
    {
      // mostrar estado del LED 23 en OLED
      Serial.println("Mostrando estado del LED 23 en OLED...");
      bot.sendMessage(chat_id, "Mostrando estado del LED 23 en OLED...", "");
    }
    else if (mensaje == "/displaypote")
    {
      // mostrar estado del potenciómetro en OLED
      Serial.println("Mostrando estado del potenciómetro en OLED...");
      bot.sendMessage(chat_id, "Mostrando estado del potenciómetro en OLED...", "");
    }
    else if (mensaje == "/displaysensor")
    {
      // mostrar estado del sensor DHT22 en OLED
      Serial.println("Mostrando estado del sensor DHT22 en OLED...");
      bot.sendMessage(chat_id, "Mostrando estado del sensor DHT22 en OLED...", "");
    }

    else
    {
      String respuesta = "Hola " + nombre_usuario + ", soy un bot que controla la placa ESP32. Puedes acceder a más información con el comando /start.";
      bot.sendMessage(chat_id, respuesta, "");
    }
  }
}

void encenderLed(int pin)
{
  digitalWrite(pin, HIGH);
}

void apagarLed(int pin)
{
  digitalWrite(pin, LOW);
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
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi conectado. Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Función para sincronizar la hora utilizando NTP
void sincronizarTiempoNTP()
{
  Serial.print("Obteniendo la hora mediante NTP...");

  configTime(0, 0, "pool.ntp.org"); // Configura NTP con UTC
  time_t ahora = time(nullptr);

  while (ahora < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    ahora = time(nullptr);
  }

  Serial.println("Tiempo sincronizado.");
}