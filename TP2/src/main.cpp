#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

void manejarEntrada(const String& input);
void manejarEntradaUsuario();

const char* WIFI_SSID  = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";
const char* BOT_TOKEN  = "7968370248:AAGJW2-cDjW_71rJGP-Z--uKA58N8ccysX4";

const unsigned long BOT_MTBS = 1000; // mean time between scan messages


const int PIN_LED = 23;

//Token de Telegram BOT se obtenienen desde Botfather en telegram
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;          // last time messages' scan has been done
bool Start = false;


void handleNewMessages(int numNewMessages)
{
  // Serial.println("handleNewMessages");
  // Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/ledon")
    {
      Serial.println("LED encendido");
      digitalWrite(PIN_LED, HIGH);
      bot.sendMessage(chat_id, "LED encendido!", "");
    } 
    else if (text == "/ledoff") 
    {
      Serial.println("LED apagado");
      digitalWrite(PIN_LED, LOW);
      bot.sendMessage(chat_id, "LED apagado!", "");
    } 
    else if (text == "/send_test_action")
    {
      bot.sendChatAction(chat_id, "typing");
      delay(4000);
      bot.sendMessage(chat_id, "Did you see the action message?");
    }

    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Chat Action Bot example.\n\n";
      welcome += "/send_test_action : to send test chat action message\n";
      bot.sendMessage(chat_id, welcome);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(PIN_LED, OUTPUT);
  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
}

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}


// void setup() {
//    Serial.begin(115200);
//    pinMode(PIN_LED, OUTPUT);

//    WiFi.begin(SSID, PASS);

//    // secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); //Agregar certificado raíz para api.telegram.org
   
//    while (WiFi.status() != WL_CONNECTED) {
//        delay(500);
//        Serial.print(".");
//    }
//    Serial.println("");
//    Serial.println("WiFi conectado");
//    Serial.println(WiFi.localIP());
// }


// // void mensajesNuevos(int numerosMensajes)
// // {
// //   for (int i = 0; i < numerosMensajes; i++)
// //   {
// //     String chat_id = bot.messages[i].chat_id;
// //     String text = bot.messages[i].text;
// //     Serial.println(text);

// //     if (text == "/ledon")
// //     {
// //       digitalWrite(PIN_LED, HIGH); // 
// //       bot.sendMessage(chat_id, "LED encendido!", "");
// //     }
 
// //     if (text == "/ledoff")
// //     {
// //       digitalWrite(PIN_LED, LOW); // 
// //       bot.sendMessage(chat_id, "LED apagado!", "");
// //     }
// //   }
// // }
 
 
// void loop()
// {
//   // manejarEntradaUsuario();
//   //Verifica si hay datos nuevos en telegram cada 1 segundo
//   // if (millis() - tiempoAnterior > tiempo)
//   // {
//   //   int numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
 
//   //   while (numerosMensajes)
//   //   {
//   //     Serial.println("Comando recibido");
//   //     mensajesNuevos(numerosMensajes);
//   //     numerosMensajes = bot.getUpdates(bot.last_message_received + 1);
//   //   }
 
//   //   tiempoAnterior = millis();
//   // }
  
// }

// void manejarEntradaUsuario() {
//   if (Serial.available()) {
//     String opcionIngresada = Serial.readStringUntil('\n');
//     opcionIngresada.trim();
//     manejarEntrada(opcionIngresada);
//   }
// }


// void manejarEntrada(const String& input) {
//   if (input == "/ledon") {
//     Serial.println("Led encendido");
//     digitalWrite(PIN_LED, HIGH);
//   } else if (input == "/ledoff") {
//     Serial.println("Led apagado");
//     digitalWrite(PIN_LED, LOW);
//   } else {
//     Serial.println("Opcion incorrecta");
//   }

// }