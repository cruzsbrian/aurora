#include "lights.h"

#include <Arduino.h>


namespace lights {

const byte pin_list[8] = {0, 1, 2, 3, 4, 5, 6, 7};

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

void setRGB(int idx, double r, double g, double b) {
    leds.setPixel(idx, r * 255, g * 255, b * 255);
}

void setHSV(int idx, double h, double s, double v) {
    double r, g, b;
    hsv2rgb(h, s, v, &r, &g, &b);
    setRGB(idx, r, g, b);
}

void getRGB(int idx, double *r, double *g, double *b) {
    uint32_t rgb = leds.getPixel(idx);
    *r = (rgb >> 16) / 255.0;
    *g = ((rgb >> 8) & 0xff) / 255.0;
    *b = (rgb & 0xff) / 255.0;
}


void hsv2rgb(double h, double s, double v, double *r, double *g, double *b) {
    float i = (int) (h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch ((int) i % 6) {
        case 0: *r = v, *g = t, *b = p; break;
        case 1: *r = q, *g = v, *b = p; break;
        case 2: *r = p, *g = v, *b = t; break;
        case 3: *r = p, *g = q, *b = v; break;
        case 4: *r = t, *g = p, *b = v; break;
        case 5: *r = v, *g = p, *b = q; break;
    }
}

}