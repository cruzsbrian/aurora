#include "ui/ui.h"


UserInterface ui;


void setup() {
    ui.init();
}


void loop() {
    ui.update();
    delay(5);
}