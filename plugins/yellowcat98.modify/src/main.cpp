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

/*
	table["registerHook"] = [](sol::this_state ts) {
		print("Register hook called", "info");
		static uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
		print("Base retrieved!", "info");
		auto targetFn = reinterpret_cast<void*>(base + 0x3207e0);
		if (!targetFn) {
			print("BAD target fn", "info");
			return;
		}
		print("Target FN retrieved!", "info");
		tulip::hook::HandlerMetadata handlerMeta;
		
		auto handlerRes = tulip::hook::createHandler(targetFn, handlerMeta);
		if (handlerRes.isErr()) {
			api.log(api.metadata, handlerRes.err().value().c_str(), "error");
			return;
		} else {
			print("Handler is fine", "info");
		}

		auto handler = handlerRes.unwrap();
		print("Created handler!", "info");

		tulip::hook::HookMetadata hookMeta;
		hookMeta.m_priority = 0;
		print("Created metadata!", "info");
		auto hook = tulip::hook::createHook(handler, &onMoreJames, hookMeta);
		print("created hook", "info");
		if (!hook) {
			api.log(api.metadata, "failed to create hook", "error");
		}
		print("hook created", "info");
	};
*/

	table["registerHook"] = [](sol::this_state ts, sol::function function) {
		
		globals::hookDetours.insert({"MenuLayer_onMoreGames", function}); // yes youre not supposed to call registerHook more than once otherwise this will get called twice and do problemos

		geode::Mod::get()->hook(
			reinterpret_cast<void*>(geode::base::get() + 0x320880),
			+[](void* self, void* sender) {
				sol::state_view state(globals::rawState);

				auto func = globals::hookDetours["MenuLayer_onMoreGames"];

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