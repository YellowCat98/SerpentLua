#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void ScriptBuiltin::entry(lua_State* L) {
    sol::state_view state(L);

    auto t = state.new_usertype<ScriptMetadata>("ScriptMetadata", sol::no_constructor);

    t["getByID"] = &ScriptMetadata::getScript;
    t["name"] = &ScriptMetadata::name;
    t["id"] = &ScriptMetadata::id;
    t["version"] = &ScriptMetadata::version;
    t["serpentVersion"] = &ScriptMetadata::serpentVersion;
    t["nostd"] = &ScriptMetadata::nostd;
    
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