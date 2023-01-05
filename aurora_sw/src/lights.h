#pragma once

#include <OctoWS2811.h>


#define N_STRIPS 2
#define LEDS_PER_STRIP 360
#define N_LEDS 660


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