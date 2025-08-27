#pragma once
#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp> // since the builtin plugin isnt in internal/SerpentLua.hpp, its no longer a part of the serpentlua core so i can include sol2

namespace SerpentLua::internal::ScriptBuiltin { // builtin plugin, can be included within a script by doing `--@plugins serpentlua_builtin`
    void entry(lua_State* L);

    inline SerpentLua::Plugin* plugin = nullptr;
    inline sol::table mainModule;
    geode::Result<> initPlugin();

    sol::table logging(sol::state_view state);
};