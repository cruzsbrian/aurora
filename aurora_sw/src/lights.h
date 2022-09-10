#pragma once

#include <OctoWS2811.h>


#define N_STRIPS 1
#define LEDS_PER_STRIP 60
#define N_LEDS (N_STRIPS * LEDS_PER_STRIP)


namespace lights {

void init();
void show();
void blank();

extern OctoWS2811 leds;

}