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
	void (*test)();
    __metadata metadata;
    HMODULE handle;
};

static SerpentLuaAPI api;

__declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	api = toiletAPI;
}

__declspec(dllexport) void entry(lua_State* L) {

	lua_pushcfunction(L, [](lua_State* L) -> int {
		api.log(api.metadata, "hello", "info");
		api.log(api.metadata, "hello1", "warn");
		api.log(api.metadata, "hello2", "debug");
		api.log(api.metadata, "hello3", "error");
		return 0;
	});
	lua_setglobal(L, "the_Function");
}

#ifdef __cplusplus
}
#endif