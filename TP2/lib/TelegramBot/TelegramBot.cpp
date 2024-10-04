#include "TelegramBot.h"

TelegramBot::TelegramBot(const String &botToken, WiFiClientSecure &securedClient,
                         LEDController &led23, LEDController &led2,
                         DHT22Sensor &sensor, Potenciometro &potenciometro, Display &display)
    : bot(botToken, securedClient),
      led23(led23), led2(led2), sensor(sensor), potenciometro(potenciometro), display(display)
{
    tiempo_ultimo_mensaje = 0;
    Start = false;
}

void TelegramBot::iniciarBot()
{
    int numMensajesPendientes = bot.getUpdates(bot.last_message_received + 1);
    while (numMensajesPendientes)
    {
        numMensajesPendientes = bot.getUpdates(bot.last_message_received + 1);
    }

    bot.sendMessage("-4542379264", "🤖 Bot inicializado!", "");
}

void TelegramBot::manejarMensajesTelegram()
{
    if (millis() - tiempo_ultimo_mensaje > 1000) // 1000ms entre escaneos de mensajes
    {
        int numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1);
        while (numMensajesNuevos)
        {
            for (int i = 0; i < numMensajesNuevos; i++)
            {
                String chat_id = bot.messages[i].chat_id;
                String text = bot.messages[i].text;
                String from_name = bot.messages[i].from_name;

                if (from_name == "")
                    from_name = "Guest";

                procesarComandosTelegram(chat_id, text, from_name);
            }
            numMensajesNuevos = bot.getUpdates(bot.last_message_received + 1);
        }
        tiempo_ultimo_mensaje = millis();
    }
}

void TelegramBot::procesarComandosTelegram(const String &chat_id, const String &mensaje, const String &nombre_usuario)
{
    if (!Start && mensaje != "/start")
    {
        bot.sendMessage(chat_id, "⚠️ Usa /start para habilitar los comandos", "");
        return;
    }

    if (mensaje == "/start")
    {
        manejarComandoStart(chat_id, nombre_usuario);
    }
    else if (mensaje.startsWith("/led"))
    {
        manejarComandoLed(chat_id, mensaje);
    }
    else if (mensaje == "/dht22")
    {
        manejarComandoDHT22(chat_id);
    }
    else if (mensaje == "/pote")
    {
        manejarComandoPote(chat_id);
    }
    else if (mensaje.startsWith("/display"))
    {
        manejarComandoDisplay(chat_id, mensaje);
    }
    else if (mensaje == "/platiot")
    {
        manejarComandoPlatiot(chat_id);
    }
    else
    {
        bot.sendMessage(chat_id, "❌ Comando Invalido", "");
    }
}

void TelegramBot::manejarComandoStart(const String &chat_id, const String &nombre_usuario)
{
    Start = true;
    String welcomeMessage = "¡Hola, " + nombre_usuario + "! 🎉\n\n";
    welcomeMessage += "Qué bueno verte por acá. A continuación te dejo los comandos que podés usar:\n\n";
    welcomeMessage += "💡🟢 /led23<on/off> - Controla el LED Verde\n\n";
    welcomeMessage += "💡🔵 /led2<on/off> - Controla el LED Azul\n\n";
    welcomeMessage += "🌡️ /dht22 - Obtén lecturas del sensor DHT22\n\n";
    welcomeMessage += "🎚️ /pote - Lee el valor potenciómetro\n\n";
    welcomeMessage += "📡 /platiot - Envía datos a IoT\n\n";
    welcomeMessage += "🖥️ /display<component> - Muestra el estado del componente\n\n";
    welcomeMessage += "¡¿Listo para comenzar?! ¡Elegí el comando que vos quieras!";
    bot.sendMessage(chat_id, welcomeMessage, "");
    enviarLogSerial("Se ha iniciado la conversación con el usuario " + nombre_usuario);
}

void TelegramBot::manejarComandoLed(const String &chat_id, const String &mensaje)
{
    if (mensaje == "/led23on")
    {
        led23.encenderLed();
        bot.sendMessage(chat_id, "💡🟢 LED 23 encendido", "");
        enviarLogSerial("LED 23 encendido");
    }
    else if (mensaje == "/led23off")
    {
        led23.apagarLed();
        bot.sendMessage(chat_id, "💡🟢 LED 23 apagado", "");
        enviarLogSerial("LED 23 apagado");
    }
    else if (mensaje == "/led2on")
    {
        led2.encenderLed();
        bot.sendMessage(chat_id, "💡🔵 LED 2 encendido", "");
        enviarLogSerial("LED 2 encendido");
    }
    else if (mensaje == "/led2off")
    {
        led2.apagarLed();
        bot.sendMessage(chat_id, "💡🔵 LED 2 apagado", "");
        enviarLogSerial("LED 2 apagado");
    }
    else
    {
        bot.sendMessage(chat_id, "❌ Comando Invalido", "");
    }
}

void TelegramBot::manejarComandoDHT22(const String &chat_id)
{
    float humedad = sensor.leerHumedad();
    float temperatura = sensor.leerTemperatura();
    String mensaje = "☁️ Humedad: " + String(humedad) + "%\n";
    mensaje += "🌡️ Temperatura: " + String(temperatura) + "°C";
    bot.sendMessage(chat_id, mensaje, "");
    enviarLogSerial("Datos del sensor DHT22 enviados");
}

void TelegramBot::manejarComandoPote(const String &chat_id)
{
    float voltaje = potenciometro.leerVoltaje(3.3);
    Serial.println("Potenciómetro: " + String(voltaje) + "V");
    Serial.println("Valor potenciometro: " + String(potenciometro.leerValor()));
    String mensaje = "🎛️ Potenciómetro: " + String(voltaje) + "V";
    bot.sendMessage(chat_id, mensaje, "");
    enviarLogSerial("Datos del potenciómetro enviados");
}

void TelegramBot::manejarComandoDisplay(const String &chat_id, const String &mensaje)
{
    if (mensaje == "/displayled23")
    {
        String estado_led23 = led23.leerEstado();
        bot.sendMessage(chat_id, "💡🟢 LED 23: " + estado_led23, "");
        display.actualizarDisplay("LED 23: " + estado_led23, 5000);
        enviarLogSerial("Datos mostrados en display");
    }
    else if (mensaje == "/displayled2")
    {
        String estado_led2 = led2.leerEstado();
        bot.sendMessage(chat_id, "💡🔵 LED 2: " + estado_led2, "");
        display.actualizarDisplay("LED 2: " + estado_led2, 5000);
        enviarLogSerial("Datos mostrados en display");
    }
    else if (mensaje == "/displaypote")
    {
        float voltaje = potenciometro.leerVoltaje(3.3);
        Serial.println("Potenciómetro: " + String(voltaje) + "V");
        bot.sendMessage(chat_id, "🎛️ Potenciómetro: " + String(voltaje) + "V", "");
        display.actualizarDisplay("Potenciometro: " + String(voltaje) + "V", 5000);
        enviarLogSerial("Datos mostrados en display");
    }
    else if (mensaje == "/displaysensor")
    {
        float humedad = sensor.leerHumedad();
        float temperatura = sensor.leerTemperatura();
        String mensaje_sensor = "☁️ Humedad: " + String(humedad) + "%\n";
        mensaje_sensor += "🌡️ Temperatura: " + String(temperatura) + " C";
        bot.sendMessage(chat_id, mensaje_sensor, "");
        display.actualizarDisplay("Humedad: " + String(humedad) + "%\n" + "Temperatura: " + String(temperatura), 5000);
        enviarLogSerial("Datos mostrados en display");
    }
    else
    {
        bot.sendMessage(chat_id, "❌ Comando Display Invalido", "");
        display.actualizarDisplay("Comando Display Invalido", 5000);
    }
}

void TelegramBot::manejarComandoPlatiot(const String &chat_id)
{
    float humedad = sensor.leerHumedad();
    float temperatura = sensor.leerTemperatura();

    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, humedad);

    unsigned long channelID = 2670807;
    const char *WriteAPIKey = "5O5DX1BTSAM7TLS5";

    int responseCode = ThingSpeak.writeFields(channelID, WriteAPIKey);
    if (responseCode == 200)
    {
        bot.sendMessage(chat_id, "✅ Datos enviados a ThingSpeak!\n✅ Puedes visualizarlo en https://thingspeak.com/channels/2670807", "");
        enviarLogSerial("Datos enviados a ThingSpeak");
    }
    else
    {
        bot.sendMessage(chat_id, "❌ Error al enviar datos: " + String(responseCode), "");
        enviarLogSerial("Error al enviar datos: " + String(responseCode));
    }
}

String TelegramBot::obtenerTiempoFormateado(unsigned long millis)
{
    unsigned long seconds = millis / 1000;
    unsigned long minutes = (seconds / 60) % 60;

    return String(minutes) + "m " + String(seconds % 60) + "s | ";
}

void TelegramBot::enviarLogSerial(const String &mensaje)
{
    String tiempo_milis = obtenerTiempoFormateado(millis());
    Serial.println(tiempo_milis + mensaje);
}
