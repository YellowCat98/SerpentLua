#include <internal/std/PluginEntry.hpp>
#include <internal/std/Playground.hpp>
#include <sol/sol.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void ScriptBuiltin::entry(lua_State* L) {
    sol::state_view state(L);

    ScriptBuiltin::mainModule = state.create_table();

    auto _ScriptMetadata = ScriptBuiltin::mainModule.new_usertype<SerpentLua::ScriptMetadata>("ScriptMetadata", sol::no_constructor);

    _ScriptMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
        sol::state_view lua(ts); // ts so tuff!

        auto result = SerpentLua::ScriptMetadata::getScript(id);

        if (result.isErr()) return sol::make_object(lua, result.err().value());

        return sol::make_object(lua, result.unwrap());
    };

    _ScriptMetadata["get"] = [](sol::this_state ts) -> sol::object {
        lua_State* L = ts;
    
        for (auto& pair : RuntimeManager::get()->getAllLoadedScripts()) {
            if (pair.second->getLuaState() == L) return sol::make_object(L, pair.second->getMetadata());
        }
        return sol::nil;
    };

    _ScriptMetadata["name"] = &SerpentLua::ScriptMetadata::name;
    _ScriptMetadata["id"] = &SerpentLua::ScriptMetadata::id;
    _ScriptMetadata["version"] = &SerpentLua::ScriptMetadata::version;
    _ScriptMetadata["serpentVersion"] = &SerpentLua::ScriptMetadata::serpentVersion;
    _ScriptMetadata["nostd"] = &SerpentLua::ScriptMetadata::nostd;
    _ScriptMetadata["developer"] = &SerpentLua::ScriptMetadata::developer;
    _ScriptMetadata["path"] = &SerpentLua::ScriptMetadata::path;

    ScriptBuiltin::mainModule["log"] = logging(state);
    ScriptBuiltin::mainModule["playground"] = ScriptBuiltin::Playground::entry(state);

    state["serpentlua_modules"]["SerpentLua"] = []() {
        return ScriptBuiltin::mainModule;
    };
}

sol::table ScriptBuiltin::logging(sol::state_view state) {

    auto logging = state.create_table();

    logging["info"] = [](sol::this_state ts, const std::string& msg) -> void {
        sol::state_view state(ts);

        auto getprotected = ScriptBuiltin::mainModule["ScriptMetadata"]["get"]();

        sol::object get = getprotected;

        if (get == sol::nil) {
            log::error("Script has failed to print! ScriptMetadata.get returned nil.");
            return;
        }

        log::info("[SCRIPT] [{}]: {}", get.as<SerpentLua::ScriptMetadata*>()->name, msg);
    };

    logging["debug"] = [](sol::this_state ts, const std::string& msg) -> void {
        sol::state_view state(ts);

        auto getprotected = ScriptBuiltin::mainModule["ScriptMetadata"]["get"]();

        sol::object get = getprotected;

        if (get == sol::nil) {
            log::error("Script has failed to print! ScriptMetadata.get returned nil.");
            return;
        }

        log::debug("[SCRIPT] [{}]: {}", get.as<SerpentLua::ScriptMetadata*>()->name, msg);
    };

    logging["warn"] = [](sol::this_state ts, const std::string& msg) -> void {
        sol::state_view state(ts);

        auto getprotected = ScriptBuiltin::mainModule["ScriptMetadata"]["get"]();

        sol::object get = getprotected;

        if (get == sol::nil) {
            log::error("Script has failed to print! ScriptMetadata.get returned nil.");
            return;
        }

        log::warn("[SCRIPT] [{}]: {}", get.as<SerpentLua::ScriptMetadata*>()->name, msg);
    };

    logging["error"] = [](sol::this_state ts, const std::string& msg) -> void {
        sol::state_view state(ts);

        auto getprotected = ScriptBuiltin::mainModule["ScriptMetadata"]["get"]();

        sol::object get = getprotected;

        if (get == sol::nil) {
            log::error("Script has failed to print! ScriptMetadata.get returned nil.");
            return;
        }

        log::error("[SCRIPT] [{}]: {}", get.as<SerpentLua::ScriptMetadata*>()->name, msg);
    };

    return logging;
}

Result<> ScriptBuiltin::initPlugin() {
    if (ScriptBuiltin::plugin) return Err("Builtin plugin was already initialized.");
    std::map<std::string, std::string> map = {
        {"name", "SerpentLua"},
        {"developer", "YellowCat98"},
        {"id", "serpentlua_builtin"},
        {"version", Mod::get()->getVersion().toVString()},
        {"serpent-version", Mod::get()->getVersion().toVString()}
    };
    auto metadata = SerpentLua::PluginMetadata::create(map);

    auto res = SerpentLua::Plugin::create(metadata, ScriptBuiltin::entry);
    if (res.isErr()) return Err("{}", res.err().value());

    ScriptBuiltin::plugin = res.unwrap();

    ScriptBuiltin::plugin->setPlugin();

    return Ok();
}