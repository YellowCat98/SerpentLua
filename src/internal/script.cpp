#include <internal/SerpentLua.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;


SerpentLua::ScriptMetadata* script::getMetadata() {
    return metadata;
}

lua_State* script::getLuaState() {
    return state;
}

lua_State* script::createState(bool nostd) {
    lua_State* state = luaL_newstate();
    if (!nostd) luaL_openlibs(state);
    return state;
}

void script::terminate() {
    log::info("Script {} termination: Initialized.", metadata->id);
    auto theUnfortunate = SerpentLua::internal::RuntimeManager::get()->getScriptByID(metadata->id);
    if (theUnfortunate.isOk()) theUnfortunate.unwrap()->loaded = false;
    // this is quite sad
    // the next thing i will do is script termination
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

geode::Result<> script::execute() {
    if (luaL_dofile(this->state, this->metadata->path.c_str()) != LUA_OK) {
        auto err = Err("Script `{}` execution: \n{}\nScript has failed initial execution, will terminate for the rest of the session.", metadata->id, std::string(lua_tostring(this->state, -1)));
        this->terminate();
        return err;
    }
    return Ok();
}

geode::Result<script*, std::string> script::getLoadedScript(const std::string& id) {
    return SerpentLua::internal::RuntimeManager::get()->getLoadedScriptByID(id);
}

geode::Result<script*, std::string> script::create(ScriptMetadata* metadata) {
    lua_State* state = script::createState(metadata->nostd);
    if (!state) {
        return Err("Script `{}` creation: An error occured creating lua state.", metadata->id);
    }

    auto ret = new script();
    if (!ret) return Err("Script `{}` creation: Couldn't create.", metadata->id);
    ret->metadata = metadata;
    ret->state = state;

    log::debug("Script `{}` creation: Created successfully!", metadata->id);
    return Ok(ret);
}