#include <iostream>
#include <string>
#include <Windows.h>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

struct __metadata {
    const char* name;
    const char* developer;
    const char* id;
    const char* version;
    const char* serpentVersion;
};

struct SerpentLuaAPI {
    void (*log)(__metadata, const char*, const char*); // Basic logging function for plugins.
    __metadata metadata; // Allows access to your plugin's metadata.
    HMODULE handle; // Your Plugin's HMODULE.
};

static SerpentLuaAPI api;

// This function must be exported with `extern "C"`
extern "C" __declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	api = toiletAPI;
}

// This is the entry function. It must be exported with `extern "C"`
extern "C" __declspec(dllexport) void entry(lua_State* L) {
	api.log(api.metadata, "PluginTest initialized!", "info");

	lua_newtable(L);

	// Basic usage of the Lua C API to create a plugin.
	lua_pushcfunction(L, [](lua_State* L) -> int {
		api.log(api.metadata, "I AM NOW CALLING the_Function HELLO WORLD", "info");
		api.log(api.metadata, "LETS WARN!", "warn");
		api.log(api.metadata, "LETS ERROR!", "error");
		api.log(api.metadata, "LETS DEBUG!", "debug");
		api.log(api.metadata, "LETS TRACE!", "trace");

		return 0;
	});
	lua_setfield(L, -2, "the_Function");

	lua_pushstring(L, "My cool Varbable.");
	lua_setfield(L, -2, "coolVar");

	lua_getglobal(L, "serpentlua_modules");

	
	lua_pushvalue(L, -2);
	lua_setfield(L, -2, api.metadata.id); // always recommended to have the table as your mod's ID.

	lua_pop(L, 2);
}