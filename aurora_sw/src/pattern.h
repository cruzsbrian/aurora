#pragma once

#include <string>

#include <LuaArduino.h>


using namespace std;


class Pattern {
public:
    Pattern(string name, string filename): name(name), filename(filename) {
        loaded = false;
        L = NULL;
    }

    void update();
    void load();
    void unload();

    string name;
    string filename;
    bool loaded;
private:
    void lua_setup();
    string read_from_sd(string filename);
    void load_code(string code);

    lua_State *L;
};