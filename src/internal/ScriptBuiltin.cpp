#include <internal/SerpentLua.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

// luau ouch
#define SCRIPT_METADATA_FIELD(field)                                      \
    static int l_ScriptMetadata_##field(lua_State* L) {                   \
        auto* meta = *(SerpentLua::ScriptMetadata**)luaL_checkudata(L, 1, "ScriptMetadataMeta"); \
        lua_pushstring(L, meta->field.c_str());                           \
        return 1;                                                         \
    }

#define LOG_FUNCTION(namereal, log_func)                                            \
    static int l_log_##namereal(lua_State* L) {                                     \
        const char* msg = luaL_checkstring(L, 1);                               \
        const char* scriptName = "Unknown";                                     \
                                                                                \
        auto* rm = RuntimeManager::get();                                       \
        if (rm != nullptr) {                                                   \
            for (auto& pair : rm->getAllLoadedScripts()) {                      \
                if (pair.second && pair.second->getLuaState() == L) {           \
                    auto* meta = pair.second->getMetadata();                    \
                    if (meta) scriptName = meta->name.c_str();                  \
                    break;                                                      \
                }                                                               \
            }                                                                   \
        }                                                                       \
                                                                                \
        log_func("[{}]: {}", scriptName, msg);                                  \
        return 0;                                                               \
    }



static int l_ScriptMetadata_get(lua_State* L) {
    for (auto& pair : RuntimeManager::get()->getAllLoadedScripts()) {
        if (pair.second->getLuaState() == L) {
            SerpentLua::ScriptMetadata* meta = pair.second->getMetadata();
            if (!meta) {
                lua_pushnil(L);
                return 1;
            }

            auto** userdata = (SerpentLua::ScriptMetadata**)lua_newuserdata(L, sizeof(SerpentLua::ScriptMetadata*));
            *userdata = meta;

            luaL_getmetatable(L, "ScriptMetadata");
            lua_setmetatable(L, -2);

            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}


static int l_ScriptMetadata_getByID(lua_State* L) {
    const char* id = luaL_checkstring(L, 1);
    auto result = SerpentLua::ScriptMetadata::getScript(id);

    if (result.isErr()) {
        lua_pushstring(L, result.err().value().c_str());
        return 1;
    }

    auto meta = result.unwrap();
    auto** userdata = (SerpentLua::ScriptMetadata**)lua_newuserdata(L, sizeof(SerpentLua::ScriptMetadata*));
    *userdata = meta;

    luaL_getmetatable(L, "ScriptMetadataMeta");
    lua_setmetatable(L, -2);

    return 1;
}

SCRIPT_METADATA_FIELD(name)
SCRIPT_METADATA_FIELD(id)
SCRIPT_METADATA_FIELD(version)
SCRIPT_METADATA_FIELD(serpentVersion)
SCRIPT_METADATA_FIELD(developer)
static int l_ScriptMetadata_nostd(lua_State* L) {
    auto* meta = *(SerpentLua::ScriptMetadata**)luaL_checkudata(L, 1, "ScriptMetadataMeta");
    lua_pushboolean(L, meta->nostd);
    return 1;
}

static const luaL_Reg ScriptMetadata_methods[] = {
    {"get", l_ScriptMetadata_get},
    {"getByID", l_ScriptMetadata_getByID},
    {"name", l_ScriptMetadata_name},
    {"id", l_ScriptMetadata_id},
    {"version", l_ScriptMetadata_version},
    {"serpentVersion", l_ScriptMetadata_serpentVersion},
    {"developer", l_ScriptMetadata_developer},
    {"nostd", l_ScriptMetadata_nostd},
    {nullptr, nullptr}
};

void register_ScriptMetadata(lua_State* L) {
    luaL_newmetatable(L, "ScriptMetadataMeta");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_register(L, nullptr, ScriptMetadata_methods);
    lua_pop(L, 1);
};

LOG_FUNCTION(info, log::info)
LOG_FUNCTION(debug, log::debug)
LOG_FUNCTION(warn, log::warn)
LOG_FUNCTION(error, log::error)

static int luaopen_SerpentLua(lua_State* L) {
    lua_newtable(L);

    lua_newtable(L);
    lua_pushcfunction(L, l_ScriptMetadata_getByID, "Serpent");
    lua_setfield(L, -2, "getByID");

    lua_pushcfunction(L, l_ScriptMetadata_get, "Serpent");
    lua_setfield(L, -2, "get");

    lua_pushcfunction(L, l_ScriptMetadata_name, "Serpent");
    lua_setfield(L, -2, "name");

    lua_pushcfunction(L, l_ScriptMetadata_id, "Serpent");
    lua_setfield(L, -2, "id");

    lua_pushcfunction(L, l_ScriptMetadata_version, "Serpent");
    lua_setfield(L, -2, "version");

    lua_pushcfunction(L, l_ScriptMetadata_serpentVersion, "Serpent");
    lua_setfield(L, -2, "serpentVersion");

    lua_pushcfunction(L, l_ScriptMetadata_developer, "Serpent");
    lua_setfield(L, -2, "developer");

    lua_pushcfunction(L, l_ScriptMetadata_nostd, "Serpent");
    lua_setfield(L, -2, "nostd");

    lua_setfield(L, -2, "ScriptMetadata");

    lua_newtable(L);
    lua_pushcfunction(L, l_log_info, "SerpentLogger");
    lua_setfield(L, -2, "info");

    lua_pushcfunction(L, l_log_debug, "SerpentLogger");
    lua_setfield(L, -2, "debug");

    lua_pushcfunction(L, l_log_warn, "SerpentLogger");
    lua_setfield(L, -2, "warn");

    lua_pushcfunction(L, l_log_error, "SerpentLogger");
    lua_setfield(L, -2, "error");

    lua_setfield(L, -2, "log");

    return 1;
}

void ScriptBuiltin::entry(lua_State* L) {
     lua_getglobal(L, "package");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setglobal(L, "package");
    }

    lua_getfield(L, -1, "preload");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushvalue(L, -1);
        lua_setfield(L, -3, "preload");
    }
    lua_pushcfunction(L, luaopen_SerpentLua, "SerpentPackage");
    lua_setfield(L, -2, "SerpentLua");

    lua_pop(L, 2);
    
    register_ScriptMetadata(L);
}
Result<> ScriptBuiltin::initPlugin() {
    if (ScriptBuiltin::plugin) return Err("Builtin plugin was already initialized.");
    std::map<std::string, std::string> map = {
        {"name", "SerpentLua Builtin"},
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