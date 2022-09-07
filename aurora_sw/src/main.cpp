#include <vector>
#include <string>
#include <SD.h>

#include "ui.h"
#include "pattern.h"


using namespace std;


#define SD_CS 17


bool card_inserted = false;

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
                patterns.push_back(Pattern(name, filename));
            }
        }
        entry.close();
    }

    return patterns;
}

string read_file(File file) {
    string contents;
    while (file.available()) {
        contents += (char)file.read();
    }
    return contents;
}


UserInterface ui;


void setup() {
    Serial.begin(115200);
    Serial.println("Starting up");

    ui.init();

    // Try to initialize the SD card.
    // If inserted, load the pattern list. If not, show the card removed page.
    Serial.print(F("Initializing SD card..."));
    if (SD.begin(SD_CS)) {
        Serial.println(F("OK!"));
        card_inserted = true;
        ui.populate_root_page(get_patterns_from_sd(SD.open("/")));
    } else {
        Serial.println(F("failed!"));
        ui.card_removed();
    }
}


void loop() {
    // Detect SD card insertion/removal.
    // If inserted, load the pattern list. If not, show the card removed page.
    if (SD.mediaPresent() && !card_inserted) {
        card_inserted = true;
        ui.populate_root_page(get_patterns_from_sd(SD.open("/")));
    } else if (!SD.mediaPresent() && card_inserted) {
        card_inserted = false;
        ui.card_removed();
    }

    ui.update();

    // Update FPS counter.
    frames++;
    long current_time = millis();
    if (current_time - last_fps_update_time > fps_update_millis) {
        // Serial.print(F("FPS: "));
        // Serial.println((double) frames / ((current_time - last_fps_update_time) / 1000.0));
        frames = 0;
        last_fps_update_time = current_time;
    }

    delay(5);
}