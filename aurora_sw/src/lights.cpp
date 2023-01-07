#include "lights.h"

#include <Arduino.h>


namespace lights {

const int BYTES_PER_LED = 3;
DMAMEM byte displayMemory[N_STRIPS * LEDS_PER_STRIP * BYTES_PER_LED];
byte drawingMemory[N_STRIPS * LEDS_PER_STRIP * BYTES_PER_LED];

const int conf = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(LEDS_PER_STRIP, displayMemory, drawingMemory, conf, N_STRIPS, pin_list);


int transform_idx(int idx) {
    // Calculate strip and index in strip based on n_leds
    int strip = 0;
    while (idx >= n_leds[strip]) {
        idx -= n_leds[strip];
        strip++;
    }

    // Calculate index for OctoWS2811 based on led_starts and led_dirs
    if (led_dirs[strip] == 1) {
        idx = strip * LEDS_PER_STRIP + idx;
    } else {
        idx = strip * LEDS_PER_STRIP + n_leds[strip] - idx - 1;
    }

    return idx;
}


void init() {
    leds.begin();
    leds.show();
}

void show() {
    leds.show();
}

void blank() {
    for (int i = 0; i < N_STRIPS * LEDS_PER_STRIP; i++) {
        leds.setPixel(i, 0, 0, 0);
    }
}

void setRGB(int idx, double r, double g, double b) {
    leds.setPixel(transform_idx(idx), r * LED_BRIGHTNESS, g * LED_BRIGHTNESS, b * LED_BRIGHTNESS);
}

void setHSV(int idx, double h, double s, double v) {
    double r, g, b;
    hsv2rgb(h, s, v, &r, &g, &b);
    setRGB(idx, r, g, b);
}

void getRGB(int idx, double *r, double *g, double *b) {
    uint32_t rgb = leds.getPixel(transform_idx(idx));
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