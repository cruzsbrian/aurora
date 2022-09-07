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


class UserInterface {
public:
    void init();
    void populate_root_page(vector<Pattern> p);
    void card_removed();
    void update();
private:
    lv_obj_t *create_page(const char *name);
    lv_obj_t *create_section(lv_obj_t *page, const char *name);
    lv_obj_t *add_menu_item(lv_obj_t *section, const char *name, lv_obj_t *sub_page, bool scrollable);

    lv_disp_draw_buf_t draw_buf;
    lv_color_t buf[SCREEN_WIDTH * 10];

    lv_style_t style_header;
    lv_obj_t *menu;
};