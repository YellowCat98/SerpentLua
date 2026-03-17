#include <Geode/Geode.hpp>
#include <yellowcat98.serpentlua/include/SerpentLua.hpp> // Lua is included within the header.

using namespace geode::prelude;

struct globals {
    inline static SerpentLua::PluginMetadata* metadata = nullptr;
    inline static SerpentLua::Plugin* plugin = nullptr;
};

// This function is just copy pasted from the yellowcat98.plugintest example but with the necessary changes
void pluginEntry(lua_State* L) {
	log::info("HELLO WORLD THIS IS THE PLUGIN ENTRY!!!");

	lua_newtable(L);

	// Basic usage of the Lua C API to create a plugin.
	lua_pushcfunction(L, [](lua_State* L) -> int {
		log::info("This is an amazing Non-native plugin.");
		return 0;
	});
	lua_setfield(L, -2, "the_Function");

	lua_pushstring(L, "My cool NOT NATIVE Varbable.");
	lua_setfield(L, -2, "coolNonNativeVar");

	lua_getglobal(L, "serpentlua_modules");

	lua_pushvalue(L, -2);
	lua_setfield(L, -2, globals::metadata->id.c_str());

	lua_pop(L, 2);
}

Result<> initPlugin() {
    std::map<std::string, std::string> metadataMap = {
        {"name", Mod::get()->getName()},
        {"developer", Mod::get()->getDevelopers()[0]}, // Mod::getDeveloper doesn't exist, because this mod only has one developer
        {"id", Mod::get()->getID()},
        {"version", Mod::get()->getVersion().toNonVString()},
        {"serpent-version", "1.0.0-alpha.12"}
    };
    globals::metadata = SerpentLua::PluginMetadata::create(metadataMap);
    if (!globals::metadata) return Err("globals::metadata == nullptr");

    auto pluginRes = SerpentLua::Plugin::create(globals::metadata, pluginEntry);
    if (pluginRes.isErr()) return Err("{}", *(pluginRes.err()));
    globals::plugin = pluginRes.unwrap();

    globals::plugin->setPlugin();
    return Ok();
}

$on_mod(Loaded) {
    auto res = initPlugin();
    if (res.isErr()) log::error("Failed to load plugin: {}", *(res.err()));
}