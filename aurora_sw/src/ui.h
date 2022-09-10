#pragma once

#include <string>
#include <vector>

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Encoder.h>

#include "pattern.h"


using namespace std;


#define PIN_ENCODER_A 21
#define PIN_ENCODER_B 22
#define PIN_BUTTON 23

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

#define BG_COLOR 0x111

#define MAX_BRIGHTNESS 20


namespace ui {

void init();
void populate_root_page(const vector<Pattern> &patterns);
void card_removed();
void update();

extern unsigned int selected_pattern;

};