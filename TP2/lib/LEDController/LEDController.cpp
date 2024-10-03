#include "LEDController.h"

LEDController::LEDController(int pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void LEDController::encenderLed() {
    digitalWrite(pin, HIGH);
}

void LEDController::apagarLed() {
    digitalWrite(pin, LOW);
}

String LEDController::leerEstado() {
    return (digitalRead(pin) == HIGH) ? "Encendido" : "Apagado";
}
