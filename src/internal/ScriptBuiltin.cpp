#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

Result<> ScriptBuiltin::initPlugin() {
    if (plugin) return Err("Builtin plugin was already initialized.");
    std::map<std::string, std::string> map = {
        {"name", "SerpentLua Builtin"},
        {"id", "serpentlua_builtin"},
        {"version", Mod::get()->getVersion().toVString()},
        {"serpent-version", Mod::get()->getVersion().toVString()}
    };
    auto metadata = SerpentLua::PluginMetadata::create(map);

    auto res = SerpentLua::Plugin::create(metadata, ScriptBuiltin::entry);
    if (res.isErr()) return Err("{}", res.err().value());

    plugin = res.unwrap();

    return Ok();
}