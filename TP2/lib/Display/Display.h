#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_SH110X.h>

class Display {
public:
    Display();
    void inicializarDisplay();
    void actualizarDisplay(const String& mensaje, int tiempoVisible);

private:
    Adafruit_SH1106G display;
};

#endif
