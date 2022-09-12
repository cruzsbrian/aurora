#include <vector>
#include <string>

#include <SD.h>
#include <Metro.h>

#include "ui.h"
#include "pattern.h"
#include "lights.h"


using namespace std;


#define SD_CS 17


bool card_inserted = false;

vector<Pattern> patterns;
unsigned int current_pattern_idx = 0;
Metro pattern_metro = Metro(20);

const long fps_update_millis = 1000;
long last_fps_update_time = 0;
long frames = 0;


vector<Pattern> get_patterns_from_sd(File dir) {
    vector<Pattern> patterns;

    // Loop through all files.
    while (File entry = dir.openNextFile()) {
        if (!entry.isDirectory()) {
            string filename = entry.name();
            Serial.println(filename.c_str());

            // If it has a lua extension, add it to the list.
            if (filename.substr(filename.length() - 4) == ".lua") {
                string name = filename.substr(0, filename.length() - 4);
                patterns.push_back(Pattern(patterns.size(), name, filename));
            }
        }
        entry.close();
    }

    return patterns;
}


void load_pattern(unsigned int idx) {
    if (idx >= patterns.size()) return;

    Serial.print(F("Loading pattern "));
    Serial.println(patterns[idx].name.c_str());

    // Unload current pattern.
    patterns[current_pattern_idx].unload();

    // Load new pattern.
    patterns[idx].load();
    current_pattern_idx = idx;
}



void setup() {
    Serial.begin(115200);
    Serial.println("Starting up");

    ui::init();
    lights::init();

    // Try to initialize the SD card.
    // If inserted, load the pattern list. If not, show the card removed page.
    Serial.print(F("Initializing SD card..."));
    if (SD.begin(SD_CS)) {
        Serial.println(F("OK!"));
        card_inserted = true;
        patterns = get_patterns_from_sd(SD.open("/"));
        ui::populate_root_page(patterns);

        load_pattern(current_pattern_idx);
        pattern_metro.reset();
    } else {
        Serial.println(F("failed!"));
        ui::card_removed();
    }
}


void loop() {
    // Detect SD card insertion/removal.
    // If inserted, load the pattern list. If not, show the card removed page.
    if (SD.mediaPresent() && !card_inserted) {
        card_inserted = true;
        patterns = get_patterns_from_sd(SD.open("/"));
        ui::populate_root_page(patterns);

        load_pattern(current_pattern_idx);
        pattern_metro.reset();
    } else if (!SD.mediaPresent() && card_inserted) {
        card_inserted = false;
        ui::card_removed();

        patterns[current_pattern_idx].unload();
        current_pattern_idx = 0;
        ui::selected_pattern = 0;

        lights::blank();
    }

    ui::update();

    if (ui::selected_pattern != current_pattern_idx) {
        load_pattern(ui::selected_pattern);
    }

    if (pattern_metro.check() == 1 && current_pattern_idx < patterns.size()) {
        patterns[current_pattern_idx].update();
        lights::show();

        // Update FPS counter.
        frames++;
        long current_time = millis();
        if (current_time - last_fps_update_time > fps_update_millis) {
            // Serial.print(F("FPS: "));
            // Serial.println((double) frames / ((current_time - last_fps_update_time) / 1000.0));
            frames = 0;
            last_fps_update_time = current_time;
        }
    }

    delay(5);
}