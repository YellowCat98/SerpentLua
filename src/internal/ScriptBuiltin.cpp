#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void ScriptBuiltin::entry(lua_State* L) {
    sol::state_view state(L);

    auto table = state.create_table();

    auto _ScriptMetadata = state.new_usertype<ScriptMetadata>("ScriptMetadata", sol::no_constructor);

    _ScriptMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
        sol::state_view lua(ts); // ts so tuff!

        auto result = ScriptMetadata::getScript(id);

        if (result.isErr()) return sol::make_object(lua, result.err().value());

        return sol::make_object(lua, result.unwrap());
    };

    _ScriptMetadata["get"] = [](sol::this_state ts) -> sol::object {
        lua_State* L = ts;
    
        for (auto& pair : RuntimeManager::get()->getAllLoadedScripts()) {
            if (pair.second->getLuaState() == ts) return sol::make_object(L, pair.second->getMetadata());
        }

        return sol::nil;
    };

    _ScriptMetadata["name"] = &ScriptMetadata::name;
    _ScriptMetadata["id"] = &ScriptMetadata::id;
    _ScriptMetadata["version"] = &ScriptMetadata::version;
    _ScriptMetadata["serpentVersion"] = &ScriptMetadata::serpentVersion;
    _ScriptMetadata["nostd"] = &ScriptMetadata::nostd;


    table["ScriptMetadata"] = _ScriptMetadata;

    state["package"]["preload"]["SerpentLua"] = [table]() {
        return table;
    };
}

Result<> ScriptBuiltin::initPlugin() {
    if (ScriptBuiltin::plugin) return Err("Builtin plugin was already initialized.");
    std::map<std::string, std::string> map = {
        {"name", "SerpentLua Builtin"},
        {"id", "serpentlua_builtin"},
        {"version", Mod::get()->getVersion().toVString()},
        {"serpent-version", Mod::get()->getVersion().toVString()}
    };
    auto metadata = SerpentLua::PluginMetadata::create(map);

    auto res = SerpentLua::Plugin::create(metadata, ScriptBuiltin::entry);
    if (res.isErr()) return Err("{}", res.err().value());

    ScriptBuiltin::plugin = res.unwrap();

    return Ok();
}