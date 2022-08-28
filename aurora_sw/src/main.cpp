#include <vector>
#include <string>
#include <SD.h>

#include "ui.h"


using namespace std;


#define SD_CS 17


bool card_inserted = false;


vector<string> get_patterns_from_sd(File dir) {
    vector<string> patterns;

    // Loop through all files.
    while (File entry = dir.openNextFile()) {
        if (!entry.isDirectory()) {
            string filename = entry.name();
            
            // If it has a lua extension, add it to the list.
            if (filename.substr(filename.length() - 4) == ".lua") {
                string name = filename.substr(0, filename.length() - 4);
                patterns.push_back(name);
            }
        }
        entry.close();
    }

    return patterns;
}

UserInterface ui;


void setup() {
    ui.init();

    // Try to initialize the SD card.
    // If inserted, load the pattern list. If not, show the card removed page.
    Serial.print("Initializing SD card...");
    if (SD.begin(SD_CS)) {
        Serial.println("OK!");
        card_inserted = true;
        ui.populate_root_page(get_patterns_from_sd(SD.open("/")));
    } else {
        Serial.println("failed!");
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
    delay(5);
}