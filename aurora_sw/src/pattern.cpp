#include "pattern.h"

#include <string>

#include <Arduino.h>
#include <SD.h>

#include "lights.h"
#include "ui.h"


static int l_print(lua_State *L) {
    Serial.print("LUA: \t");

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

        if (i < nargs) {
            Serial.print("\t");
        }
    }
    Serial.println();

    return 0;
}

static int l_serial_available(lua_State *L) {
    lua_pushnumber(L, Serial.available());
    return 1;
}

static int l_add_param(lua_State *L) {
    size_t name_len;
    const char *name = lua_tolstring(L, 1, &name_len);
    double min = lua_tonumber(L, 2);
    double max = lua_tonumber(L, 3);
    double step = lua_tonumber(L, 4);
    double val = lua_tonumber(L, 5);

    Pattern *pat = (Pattern *) lua_touserdata(L, lua_upvalueindex(1));
    if (pat == NULL) {
        Serial.println("Error: pattern from add_param is null");
        return 0;
    }

    pat->add_param(name, min, max, step, val);

    return 0;
}

static int l_setRGB(lua_State *L) {
    int idx = lua_tointeger(L, 1) - 1;
    double r = lua_tonumber(L, 2);
    double g = lua_tonumber(L, 3);
    double b = lua_tonumber(L, 4);

    lights::setRGB(idx, r, g, b);

    return 0;
}

static int l_setHSV(lua_State *L) {
    int idx = lua_tointeger(L, 1) - 1;
    double h = lua_tonumber(L, 2);
    double s = lua_tonumber(L, 3);
    double v = lua_tonumber(L, 4);

    lights::setHSV(idx, h, s, v);

    return 0;
}

static int l_getRGB(lua_State *L) {
    int idx = lua_tointeger(L, 1) - 1;
    double r, g, b;

    lights::getRGB(idx, &r, &g, &b);

    lua_pushnumber(L, r);
    lua_pushnumber(L, g);
    lua_pushnumber(L, b);

    return 3;
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

    // Print stack size for debugging.
    // Serial.print("stack size: "); Serial.println(lua_gettop(L));
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


void Pattern::add_param(const char *name, double min, double max, double step, double val) {
    Serial.print("Adding param: "); Serial.println(name);
    Serial.print("min: "); Serial.println(min);
    Serial.print("max: "); Serial.println(max);
    Serial.print("step: "); Serial.println(step);
    Serial.print("val: "); Serial.println(val);

    if (params.find(name) != params.end()) {
        Serial.println("param already exists, setting to stored value");
        set_variable(name, params[name].val);
        return;
    }

    params[name] = Param{this, name, min, max, step, val};
    ui::add_param(idx, name, params[name]);

    // Set initial value in lua.
    set_variable(name, val);
}


void Pattern::set_variable(const string &name, double val) {
    if (L == NULL) return;

    Serial.print("Setting "); Serial.print(name.c_str()); Serial.print(" to "); Serial.println(val);
    lua_pushnumber(L, val);
    lua_setglobal(L, name.c_str());
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

    // Push total number of LEDs.
    lua_pushnumber(L, N_LEDS);
    lua_setglobal(L, "N_LEDS");

    // Register the setRGB, setHSV, and getRGB functions.
    lua_pushcfunction(L, l_setRGB);
    lua_setglobal(L, "setRGB");
    lua_pushcfunction(L, l_setHSV);
    lua_setglobal(L, "setHSV");
    lua_pushcfunction(L, l_getRGB);
    lua_setglobal(L, "getRGB");

    // Register the print function.
    lua_pushcfunction(L, l_print);
    lua_setglobal(L, "print");

    // Register the add_param function.
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, l_add_param, 1);
    lua_setglobal(L, "add_param");
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