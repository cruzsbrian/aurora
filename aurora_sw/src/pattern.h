#pragma once

#include <string>
#include <unordered_map>

#include <LuaArduino.h>


using namespace std;


struct Param {
    void *pat;
    string name;
    int min, max, step, val;
};


class Pattern {
public:
    Pattern(int idx, string name, string filename):
        idx(idx), name(name), filename(filename) {
        loaded = false;
        L = NULL;
    }

    void update();
    void load();
    void unload();
    void add_param(const char *name, int min, int max, int step, int val);
    void set_variable(const string &name, int val);

    int idx;
    string name;
    string filename;
    bool loaded;
private:
    void lua_setup();
    string read_from_sd(string filename);
    void load_code(string code);

    lua_State *L;
    unordered_map<string, Param> params;
};