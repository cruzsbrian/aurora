#include "ui.h"

#include <Arduino.h>
#include <lvgl.h>


using namespace std;


TFT_eSPI tft(SCREEN_WIDTH, SCREEN_HEIGHT);
Encoder enc(PIN_ENCODER_A, PIN_ENCODER_B);

static lv_group_t *g;
static lv_indev_drv_t indev_drv;

lv_obj_t *root_page;
lv_obj_t *pattern_page;

int brightness = MAX_BRIGHTNESS / 2;

static void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data);
static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void scroll_on_focus_cb(lv_event_t *e);
void set_brightness_cb(lv_event_t *e);


void UserInterface::init() {
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    lv_init();
    tft.begin();
    tft.setRotation(2);

    analogWrite(TFT_BL, 255 * brightness * brightness / (MAX_BRIGHTNESS * MAX_BRIGHTNESS));

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * 10);

    // Initialize the display
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;
    disp_drv.ver_res = SCREEN_HEIGHT;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize the input device
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_ENCODER;
    indev_drv.read_cb = encoder_read;
    lv_indev_t *indev_encoder = lv_indev_drv_register(&indev_drv);

    // Create default group with input device
    g = lv_group_create();
    lv_group_set_default(g);
    lv_indev_set_group(indev_encoder, g);

    // Setup style for headers
    lv_style_init(&style_header);
    lv_style_set_text_font(&style_header, &lv_font_montserrat_18);

    // Create menu
    menu = lv_menu_create(lv_scr_act());
    lv_obj_add_style(lv_menu_get_main_header(menu), &style_header, 0);
    lv_obj_set_style_bg_color(menu, lv_color_hex3(BG_COLOR), 0);
    lv_obj_set_size(menu, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_center(menu);

    // Create empty pages
    root_page = create_page(NULL);
    pattern_page = create_page(NULL);

    // populate_root_page(pattern_names);

    lv_menu_set_page(menu, root_page);
    Serial.println("setup done");
}


void UserInterface::populate_root_page(vector<string> pattern_names) {
    lv_obj_clean(root_page);
    
    // Set layout/style to start from top right.
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(root_page, 0, 0);

    // Create pattern section
    lv_obj_t *pattern_section = create_section(root_page, "PATTERNS");

    // Add patterns to menu
    for (auto pattern_name : pattern_names) {
        add_menu_item(pattern_section, pattern_name.c_str(), NULL, true);
    }

    // Create settings section
    lv_obj_t *settings_section = create_section(root_page, "SETTINGS");

    // Brightness slider
    lv_obj_t *cont = add_menu_item(settings_section, LV_SYMBOL_EYE_OPEN "  ", NULL, false);
    lv_obj_set_style_pad_right(cont, 20, 0);
    lv_obj_t *slider = lv_slider_create(cont);
    lv_slider_set_range(slider, 0, MAX_BRIGHTNESS);
    lv_slider_set_value(slider, brightness, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider, set_brightness_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(slider, scroll_on_focus_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_set_flex_grow(slider, 1);

    // Setting focus to the slider (the last element) and then next element resets focus to the first element.
    // This has to be done to get focus back on an existing item if the page has been re-populated.
    lv_group_focus_obj(slider);
    lv_group_focus_next(g);
}


void UserInterface::card_removed() {
    lv_obj_clean(root_page);

    // Set page style/layout to center the items and pad between them.
    lv_obj_set_flex_align(root_page, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(root_page, 20, 0);

    // Add big SD card icon.
    lv_obj_t *card_label = lv_label_create(root_page);
    lv_label_set_text(card_label, LV_SYMBOL_SD_CARD);
    lv_obj_set_style_text_font(card_label, &lv_font_montserrat_32, 0);

    // Add label.
    lv_obj_t *insert_card_label = lv_label_create(root_page);
    lv_label_set_text(insert_card_label, "Insert SD Card");

    // Set page to root in case it's currently on a pattern page.
    // Setting page to NULL first resets the menu history, so the back button won't be displayed.
    lv_menu_set_page(menu, NULL);
    lv_menu_set_page(menu, root_page);
}


void UserInterface::update() {
    lv_timer_handler();
}


lv_obj_t *UserInterface::create_page(char *name) {
    lv_obj_t *page = lv_menu_page_create(menu, name);
    lv_obj_set_style_pad_hor(page, 5, 0);
    lv_obj_set_style_pad_bottom(page, 5, 0);
    return page;
}


lv_obj_t *UserInterface::create_section(lv_obj_t *page, const char *name) {
    lv_obj_t *cont = lv_menu_cont_create(page);

    // Make header label for the section
    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, name);
    lv_obj_add_style(label, &style_header, 0);

    return lv_menu_section_create(page);
}


lv_obj_t *UserInterface::add_menu_item(lv_obj_t *section, const char *name, lv_obj_t *sub_page, bool scrollable) {
    lv_obj_t *cont = lv_menu_cont_create(section);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);    // clearing this prevents the container from being "exited" on long press

    if (scrollable) {
        lv_group_add_obj(g, cont);  // must be added to the group to receive encoder input
        lv_obj_add_event_cb(cont, scroll_on_focus_cb, LV_EVENT_FOCUSED, NULL);  // add callback for custom scroll on focus
    }

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, name);

    if (sub_page) lv_menu_set_load_page_event(menu, cont, sub_page);

    return cont;
}


static void encoder_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    if (abs(enc.read()) >= 4) {
        data->enc_diff = enc.read() / 4;
        enc.write(0);
    }

    if (!digitalRead(PIN_BUTTON)) data->state = LV_INDEV_STATE_PRESSED;
    else data->state = LV_INDEV_STATE_RELEASED;
}


static void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}


void scroll_on_focus_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);

    lv_area_t coords = obj->coords;
    int y_center = (coords.y1 + coords.y2) / 2;
    int dist_to_center = SCREEN_HEIGHT / 2 - y_center;

    int current_scroll = lv_obj_get_scroll_y(root_page);
    lv_obj_scroll_to(root_page, 0, current_scroll - dist_to_center, LV_ANIM_ON);
}


void set_brightness_cb(lv_event_t *e) {
    lv_obj_t *obj = lv_event_get_target(e);
    brightness = lv_slider_get_value(obj);
    analogWrite(TFT_BL, 255 * brightness * brightness / (MAX_BRIGHTNESS * MAX_BRIGHTNESS));
}