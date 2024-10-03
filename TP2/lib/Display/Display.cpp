#include "display.h"
#include <Wire.h>
#include <Adafruit_SH110X.h>

#define I2C_ADDRESS 0x3C

Display::Display() : display(128, 64, &Wire, -1) {}

void Display::inicializarDisplay() {
    if (!display.begin(I2C_ADDRESS, true)) {
        Serial.println("Error al iniciar el display");
        while (true);
    }
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
}

void Display::actualizarDisplay(const String& mensaje, int tiempoVisible) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(mensaje);
    display.display();
    if (tiempoVisible > 0) {
        delay(tiempoVisible);
        display.clearDisplay();
    }
}

