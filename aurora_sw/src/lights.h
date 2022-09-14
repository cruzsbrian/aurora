#pragma once

#include <OctoWS2811.h>


#define N_STRIPS 1
#define LEDS_PER_STRIP 60
#define N_LEDS (N_STRIPS * LEDS_PER_STRIP)


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