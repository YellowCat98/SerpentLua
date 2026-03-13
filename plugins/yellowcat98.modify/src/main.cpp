#include <iostream>
#include <string>
#include <Windows.h>
#include <sol/sol.hpp>
#include <Geode/Geode.hpp>
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

void onMoreJames(void* self, void* hi) {

}

__declspec(dllexport) void initNativeAPI(SerpentLuaAPI toiletAPI) {
	api = toiletAPI;
}

void print(const std::string& msg, const char* type) {
	api.log(api.metadata, msg.c_str(), type);
}

// this struct only exists so i can retrieve things within functions that take lambdas that dont support capture
struct globals {
	static lua_State* rawState;
	static std::unordered_map<std::string, sol::function> hookDetours;
};

lua_State* globals::rawState = nullptr; // initialize as nullptr
std::unordered_map<std::string, sol::function> globals::hookDetours = {};

__declspec(dllexport) void entry(lua_State* L) {
	api.log(api.metadata, "Modify initialized!", "info");

	globals::rawState = L; // but is it now initialized as nullptr? i Dont fucking KNOW!!!!!!!!

	sol::state_view state(globals::rawState);

	auto table = state.create_table();

	table["the_Function"] = [](sol::this_state ts) {
		api.log(api.metadata, "hello World, this is the Sol2 LIBrare.", "info");
	};

	table["hook"] = [](sol::this_state ts, std::string fn, sol::function function) {
		
		
		if (globals::hookDetours.contains(fn)) {
			api.log(api.metadata, "registerHook must not be called more than once.", "warn");
			return;
		}
		globals::hookDetours.insert({fn, function}); // yes youre not supposed to call registerHook more than once otherwise this will get called twice and do problemos

		geode::Mod::get()->hook(
			reinterpret_cast<void*>(geode::base::get() + 0x335740), // i should probably add another map that holds what the functions correspond to
			+[](MenuLayer* self, cocos2d::CCObject* sender) {
				sol::state_view state(globals::rawState);

				auto func = globals::hookDetours["MenuLayer_onMoreGames"];

				sol::environment env(state, sol::create, state.globals());
				env["original"] = [](MenuLayer* self, cocos2d::CCObject* sender) {
					return self->onMoreGames(sender);
				};

				sol::set_environment(env, func);
				func(self, sender);
			},
			"Boi",
			tulip::hook::TulipConvention::Thiscall
		);
	};

	state["serpentlua_modules"][std::string(api.metadata.id)] = [table]() {
		return table;
	};
}

#ifdef __cplusplus
}
#endif