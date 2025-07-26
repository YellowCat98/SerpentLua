#include <SerpentLua.hpp>
#include <Geode/Geode.hpp>

namespace SerpentLua {
    SERPENTLUA_DLL lua_State* getLuaState() {
        static lua_State* state = nullptr;
        if (!state) state = luaL_newstate();
        return state;
    }
    SERPENTLUA_DLL void scriptLog() {
        geode::log::info("HELLO...");
    }
}