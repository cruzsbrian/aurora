#include "lights.h"

#include <Arduino.h>


namespace lights {

const byte pin_list[N_STRIPS] = {0};

const int BYTES_PER_LED = 3;
DMAMEM byte displayMemory[N_STRIPS * LEDS_PER_STRIP * BYTES_PER_LED];
byte drawingMemory[N_STRIPS * LEDS_PER_STRIP * BYTES_PER_LED];

const int conf = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(LEDS_PER_STRIP, displayMemory, drawingMemory, conf, N_STRIPS, pin_list);


void init() {
    leds.begin();
    leds.show();
}

void show() {
    leds.show();
}

void blank() {
    for (int i = 0; i < N_LEDS; i++) {
        leds.setPixel(i, 0, 0, 0);
    }
}


}