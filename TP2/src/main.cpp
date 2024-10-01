#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>


// Definición de constantes y variables globales
const char* WIFI_SSID  = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* BOT_TOKEN  = "7968370248:AAGJW2-cDjW_71rJGP-Z--uKA58N8ccysX4";
const unsigned long BOT_MTBS = 500; // Tiempo entre escaneos de mensajes (en milisegundos)
const int PIN_LED = 23;


WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long tiempo_ultimo_mensaje ; // Tiempo del último escaneo de mensajes
bool Start = false;


// Funciones declaradas
void conectarWifi();
void sincronizarTiempoNTP();
void manejarMensajesTelegram(int numMensajesNuevos);
void configurarBotTelegram();
void procesarComandosTelegram(const String& chat_id, const String& mensaje, const String& nombre_usuario);
void encenderLed();
void apagarLed();


void manejarMensajesTelegram(int numMensajesNuevos) {
  for (int i = 0; i < numMensajesNuevos; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    if (from_name == "")
      from_name = "Guest";

    procesarComandosTelegram(chat_id, text, from_name);  // Procesa los comandos recibidos
  }
}

void setup() {
  Serial.begin(115200);  // Inicializa el monitor serial
  pinMode(PIN_LED, OUTPUT);  // Configura el pin del LED como salida

  conectarWifi();  // Conecta a la red Wi-Fi
  sincronizarTiempoNTP();  // Sincroniza la hora con NTP
  // configurarBotTelegram();  // Configura el bot de Telegram

}

void loop() {
  if (millis() - tiempo_ultimo_mensaje  > BOT_MTBS) {
    int numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1);

    while (numMensajesNuevos) {
      manejarMensajesTelegram(numMensajesNuevos); // Procesa los mensajes nuevos
      numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1); // Actualiza el contador de mensajes
    }
    tiempo_ultimo_mensaje  = millis();
  }
}


void procesarComandosTelegram(const String& chat_id, const String& mensaje, const String& nombre_usuario) {
  if (mensaje == "/ledon") {
    encenderLed();
    Serial.println("LED encendido");
    bot.sendMessage(chat_id, "Led encendido", "");
  }
  else if (mensaje == "/ledoff") {
    apagarLed();
    Serial.println("LED apagado");
    bot.sendMessage(chat_id, "Led apagado", "");
  }
  else {
    String respuesta = "Hola " + nombre_usuario + ", soy un bot que controla un led. Puedes encenderlo con /ledon y apagarlo con /ledoff";
    bot.sendMessage(chat_id, respuesta, "");
  }
}

void encenderLed() {
  digitalWrite(PIN_LED, HIGH);
}

void apagarLed() {
  digitalWrite(PIN_LED, LOW);
}

// Función para conectarse a la red Wi-Fi
void conectarWifi() {
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
void sincronizarTiempoNTP() {
  Serial.print("Obteniendo la hora mediante NTP...");

  configTime(0, 0, "pool.ntp.org");  // Configura NTP con UTC
  time_t ahora = time(nullptr);

  while (ahora < 24 * 3600) {
    Serial.print(".");
    delay(100);
    ahora = time(nullptr);
  }

  Serial.println("\n\nTiempo sincronizado.");
}