#include <internal/SerpentLua.hpp>
#include "SerpentLua.hpp"

using namespace SerpentLua::internal;
using namespace geode::prelude;


SerpentLua::ScriptMetadata* script::getMetadata() {
    return metadata;
}

lua_State* script::getLuaState() {
    return state;
}

lua_State* script::createState() {
    log::debug("Script {} state creation: Initialized.", this->metadata->id);
    lua_State* state = luaL_newstate();
    if (!this->metadata->nostd) {

        luaL_requiref(state, "_G", luaopen_base, 1);
        // nil out the bad guys from _G before continuing loading!

        lua_pushnil(state); lua_setglobal(state, "dofile");
        lua_pushnil(state); lua_setglobal(state, "loadfile");
        lua_pushnil(state); lua_setglobal(state, "require");
        lua_pushnil(state); lua_setglobal(state, "load");


        luaL_requiref(state, LUA_MATHLIBNAME, luaopen_math, 1);
        luaL_requiref(state, LUA_TABLIBNAME, luaopen_table, 1);
        luaL_requiref(state, LUA_COLIBNAME, luaopen_coroutine, 1);
        luaL_requiref(state, LUA_STRLIBNAME, luaopen_string, 1);
        lua_pop(state, 5);

        lua_newtable(state);
        lua_setglobal(state, "serpentlua_modules");

        lua_pushcfunction(state, [](lua_State* L) -> int {
            const char* module = luaL_checkstring(L, 1);

            lua_getglobal(L, "serpentlua_modules");
            if (!lua_istable(L, -1)) {
                return luaL_error(L, "serpentlua_modules is not defined.");
            }

            lua_getfield(L, -1, module);

            if (lua_isnil(L, -1)) {
                return luaL_error(L, "Module %s was not found.", module);
            }

            if (lua_isfunction(L, -1)) {
                lua_pushvalue(L, -1);
                lua_call(L, 0, 1);

                lua_pushvalue(L, -1);
                lua_getglobal(L, "serpentlua_modules");

                lua_insert(L, -2);

                lua_setfield(L, -1, module);

                lua_pop(L, 1);
            }

            return 1;
        });
        lua_setglobal(state, "serpentlua_require");

    }

    lua_atpanic(state, [](lua_State* L) -> int {
        log::error("LUA PANIC: {}", lua_tostring(L, -1));
        return 0;
    });

    return state;
}

void script::terminate() {
    log::info("Script {} termination: Initialized.", metadata->id);
    // this is quite sad
    // the next thing i will do is script termination
    auto res = RuntimeManager::get()->removeLoadedScript(this->metadata->id); // maybe we should remove it from loaded scripts too!
    if (res.isErr()) log::error("Script {} termination: ", res.err().value());
    delete this;
    // will thi even compiling
    // ok c/c++ extension thinks it will compile
    // it did not compile
    // it turns out it was just a fmt error
    // in line 21, i forgot the arguments
    // i will fix and see now.
    // it did not compile once again, it was main.cpp(62,37)
    // i will check 
    // ok now it should compile
    // it compiled!! :D
}

geode::Result<> script::loadPlugins() {
    for (auto& pluginID : this->metadata->plugins) {
        auto pluginRes = RuntimeManager::get()->getLoadedPluginByID(pluginID);
        if (pluginRes.isErr()) {
            auto err = Err("Script `{}` plugin loading: Plugin getter returned an error:\n\n{}\n\nWill terminate for the rest of this session.", this->metadata->id, pluginRes.err().value());
            this->terminate();
            return err;
        }
        auto unwrapped = pluginRes.unwrap();
        unwrapped->getEntry()(this->getLuaState());

        pendingPlugins.push_back(unwrapped);
    }

    return Ok();
}

// only terminate when a script fails inital execution, it will crash if anything after initial execution fails, this is to prevent crashes!
geode::Result<> script::execute() {
    if (luaL_dofile(this->state, this->metadata->path.c_str()) != LUA_OK) {
        auto err = Err("Script `{}` execution: \n\n{}\n\nScript has failed initial execution, will terminate for the rest of this session.", metadata->id, std::string(lua_tostring(this->state, -1)));
        this->terminate();
        return err;
    }
    metadata->loaded = true;
    this->commitLoadedPlugins(); // this code is SUPPOSED to only be reached after plugins were loaded in
    return Ok();
}

void script::commitLoadedPlugins() {
    for (auto& plugin : pendingPlugins) {
        plugin->loadCount++;
    }
}

geode::Result<script*, std::string> script::getLoadedScript(const std::string& id) {
    return SerpentLua::internal::RuntimeManager::get()->getLoadedScriptByID(id);
}

geode::Result<script*, std::string> script::create(ScriptMetadata* metadata) {

    auto ret = new script();
    if (!ret) return Err("Script `{}` creation: Couldn't create.", metadata->id);
    ret->metadata = metadata;

    ret->state = ret->createState();
    if (!ret->state) return Err("Script `{}` creation: Lua State is nullptr.", metadata->id);

    log::debug("Script `{}` creation: Created successfully!", metadata->id);
    return Ok(ret);
}