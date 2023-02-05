#pragma once

#include <OctoWS2811.h>


// Hell-Lighting config
#define LED_BRIGHTNESS 50  // out of 255

#define N_STRIPS 4
#define LEDS_PER_STRIP 356
#define N_LEDS 1220

const int n_leds[N_STRIPS] = {285, 349, 254, 332};
const int led_dirs[N_STRIPS] = {1, -1, -1, 1};


// Brian's room config
// #define LED_BRIGHTNESS 255  // out of 255

// #define N_STRIPS 2
// #define LEDS_PER_STRIP 360
// #define N_LEDS 660

// const int n_leds[N_STRIPS] = {300, 360};
// const int led_dirs[N_STRIPS] = {1, -1};


const byte pin_list[8] = {0, 1, 2, 3, 4, 5, 6, 7};


namespace lights {

void init();
void show();
void blank();
void setRGB(int idx, double r, double g, double b);
void setHSV(int idx, double h, double s, double v);
void getRGB(int idx, double *r, double *g, double *b);

void hsv2rgb(double h, double s, double v, double *r, double *g, double *b);

extern OctoWS2811 leds;

}