#pragma once

#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Encoder.h>


#define PIN_ENCODER_A 21
#define PIN_ENCODER_B 22
#define PIN_BUTTON 23

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

#define BG_COLOR 0x111


class UserInterface {
public:
    void init();
    void update();
    void (*pattern_change_cb)(int pattern_idx);
private:
    void add_pattern(const char *name, int pattern_idx);
    lv_obj_t *create_page(char *name);
    lv_obj_t *create_section(lv_obj_t *page, const char *name);
    lv_obj_t *add_menu_item(lv_obj_t *section, const char *name, lv_obj_t *sub_page, bool scrollable);

    lv_disp_draw_buf_t draw_buf;
    lv_color_t buf[SCREEN_WIDTH * 10];

    lv_style_t style_header;
    lv_obj_t *menu;
    lv_obj_t *pattern_section;
    lv_obj_t *settings_section;
};