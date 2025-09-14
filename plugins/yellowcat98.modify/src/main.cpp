#include <iostream>
#include <string>
#include <Windows.h>
#include <sol/sol.hpp>
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

	sol::state_view state(L);

	auto table = state.create_table();

	table["the_Function"] = [](sol::this_state ts) {
		api.log(api.metadata, "hello World, this is the Sol2 LIBrare.", "info");
	};

	state["serpentlua_modules"][std::string(api.metadata.id)] = [table]() {
		return table;
	};
}

#ifdef __cplusplus
}
#endif