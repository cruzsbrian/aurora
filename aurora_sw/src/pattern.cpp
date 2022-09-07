#include "pattern.h"

#include <string>

#include <Arduino.h>
#include <SD.h>


#define LED_COUNT 10


void Pattern::update() {
    if (!loaded) return;

    // Get and call pattern update function.
    lua_getglobal(L, "update");

    int error = lua_pcall(L, 0, 0, 0);
    if (error) {
        Serial.print("ERROR: ");
        Serial.println(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // Print led array.
    lua_getglobal(L, "leds");

    for (int i = 0; i < LED_COUNT; i++) {
        lua_rawgeti(L, -1, i + 1);
        Serial.print(lua_tointeger(L, -1));
        Serial.print(" ");
        lua_pop(L, 1);
    }
    Serial.println();

    // Pop led array.
    lua_pop(L, 1);

    // Print stack size for debugging.
    // Serial.println(lua_gettop(L));
}


void Pattern::load() {
    lua_setup();
    load_code(read_from_sd(filename));
}



void Pattern::unload() {
    if (L) {
        Serial.println(F("Unloading pattern."));
        lua_close(L);
        L = NULL;
        loaded = false;
    }
}


string Pattern::read_from_sd(string filename) {
    File file = SD.open(filename.c_str());
    if (!file.available()) {
        Serial.println("ERROR: Could not open file.");
        return "";
    }

    string contents;
    while (file.available()) {
        contents += (char)file.read();
    }
    file.close();

    return contents;
}


void Pattern::lua_setup() {
    Serial.println(F("Opening new lua state."));
    L = lua_open();
    luaL_openlibs(L);

    // Set up the LED array.
    Serial.println(F("Creating the leds array."));
    lua_newtable(L);
    for (int i = 0; i < LED_COUNT; i++) {
        lua_pushnumber(L, 0);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "leds");
}


void Pattern::load_code(string code) {
    // If there's an error loading the pattern, loaded will remain false.
    loaded = false;

    Serial.println(F("Pattern code:"));
    Serial.println(code.c_str());

    // Load the pattern code.
    Serial.println(F("Loading pattern code."));
    int error = luaL_loadstring(L, code.c_str());
    if (error) {
        Serial.print(F("ERROR: "));
        Serial.println(lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    // Execute code (so update and init functions can be called later).
    Serial.println(F("Executing pattern code."));
    error = lua_pcall(L, 0, 0, 0);
    if (error) {
        Serial.print(F("ERROR: "));
        Serial.println(lua_tostring(L, -1));
        lua_pop(L, 1);
        return;
    }

    loaded = true;
}