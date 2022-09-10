#include "pattern.h"

#include <string>

#include <Arduino.h>
#include <SD.h>

#include "lights.h"



void hsv2rgb(float h, float s, float v, byte *r, byte *g, byte *b) {
    float i = (int) (h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    float rf, gf, bf;

    switch ((int) i % 6) {
        case 0: rf = v, gf = t, bf = p; break;
        case 1: rf = q, gf = v, bf = p; break;
        case 2: rf = p, gf = v, bf = t; break;
        case 3: rf = p, gf = q, bf = v; break;
        case 4: rf = t, gf = p, bf = v; break;
        case 5: rf = v, gf = p, bf = q; break;
    }

    *r = (byte) (rf * 255);
    *g = (byte) (gf * 255);
    *b = (byte) (bf * 255);
}

static int l_hsv2rgb(lua_State *L) {
    float h = lua_tonumber(L, 1);
    float s = lua_tonumber(L, 2);
    float v = lua_tonumber(L, 3);

    byte r, g, b;
    hsv2rgb(h, s, v, &r, &g, &b);

    lua_newtable(L);
    lua_pushinteger(L, r);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, g);
    lua_rawseti(L, -2, 2);
    lua_pushinteger(L, b);
    lua_rawseti(L, -2, 3);

    return 1;
}

static int l_print(lua_State *L) {
    Serial.print("LUA: ");

    int nargs = lua_gettop(L);

    for (int i = 1; i <= nargs; i++) {
        if (lua_isnumber(L, i)) {
            Serial.print(lua_tonumber(L, i));
        } else if (lua_isstring(L, i)) {
            size_t l;
            const char *s = lua_tolstring(L, i, &l);
            Serial.write(s, l);
        } else {
            Serial.print("?");
        }
    }
    Serial.println();

    return 0;
}



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

    // Read LED values from lua and set them in OctoWS2811.
    lua_getglobal(L, "leds");

    for (int i = 0; i < N_LEDS; i++) {
        lua_rawgeti(L, -1, i + 1);  // Get LED at index i + 1 from table (-1 in stack).
        // Serial.print(lua_tointeger(L, -1));
        // Serial.print(" ");

        // Get R value.
        lua_rawgeti(L, -1, 1);
        int r = lua_tointeger(L, -1);
        lua_pop(L, 1);

        // Get G value.
        lua_rawgeti(L, -1, 2);
        int g = lua_tointeger(L, -1);
        lua_pop(L, 1);

        // Get B value.
        lua_rawgeti(L, -1, 3);
        int b = lua_tointeger(L, -1);
        lua_pop(L, 1);

        lights::leds.setPixel(i, r, g, b);

        lua_pop(L, 1);  // Pop LED.
    }
    // Serial.println();

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
        Serial.print(F("Unloading pattern "));
        Serial.println(name.c_str());
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
    for (int i = 0; i < N_LEDS; i++) {
        lua_newtable(L);
        for (int i = 0; i < 3; i++) {
            lua_pushinteger(L, 0);
            lua_rawseti(L, -2, i + 1);
        }
        lua_rawseti(L, -2, i + 1);
    }
    lua_setglobal(L, "leds");

    // Register the hsv2rgb function.
    lua_pushcfunction(L, l_hsv2rgb);
    lua_setglobal(L, "HSV");

    // Register the print function.
    lua_pushcfunction(L, l_print);
    lua_setglobal(L, "myprint");
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