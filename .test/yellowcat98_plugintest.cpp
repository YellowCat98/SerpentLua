#include <iostream>
#ifdef __cplusplus
extern "C" {
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


__declspec(dllexport) extern const struct __metadata {
	const char* name;
	const char* id;
	const char* version;
	const char* serpentVersion;
} plugin_metadata;

__declspec(dllexport) const struct __metadata plugin_metadata = {
	"PluginTest",
	"yellowcat98_plugintest",
	"1.0.0",
	"1.0.0"
};

struct __declspec(dllexport) __script_metadata {
    const char* name;
    const char* id;
    const char* version;
    const char* serpentVersion;
    const bool nostd;
    const char* path;
};

__declspec(dllexport) void entry(lua_State* L) {
	lua_pushcfunction(L, [](lua_State* L) -> int {
		std::cout << "hello world, this is the Dll." << std::endl;
		return 0;
	});
	lua_setglobal(L, "the_Function");
}

#ifdef __cplusplus
}
#endif