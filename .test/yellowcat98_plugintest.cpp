#include <iostream>
#include <string>
#include <Windows.h>
#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

struct __metadata {
    const char* name;
    const char* developer;
    const char* id;
    const char* version;
    const char* serpentVersion;
};

struct SerpentLuaAPI {
    void (*log)(__metadata, const char*, const char*);
    __metadata metadata;
    HMODULE handle;
};

static SerpentLuaAPI api;

__declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	api = toiletAPI;
}

__declspec(dllexport) void entry(lua_State* L) {
	api.log(api.metadata, "PluginTest initialized!", "info");

	lua_newtable(L);

	lua_pushcfunction(L, [](lua_State* L) -> int {
		api.log(api.metadata, "WELCOME TO MY TEST PLUGIN!.", "info");
		return 0;
	});
	lua_setfield(L, -2, "the_Function");

	lua_pushstring(L, "My cool Varbable.");
	lua_setfield(L, -2, "coolVar");

	lua_getglobal(L, "serpentlua_modules");

	
	lua_pushvalue(L, -2);
	lua_setfield(L, -2, api.metadata.id); // always recommended to have the table as your mod's ID.
}

#ifdef __cplusplus
}
#endif