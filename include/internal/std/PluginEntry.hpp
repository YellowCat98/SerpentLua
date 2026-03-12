#pragma once
#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp> // since the builtin plugin isnt in internal/SerpentLua.hpp, its no longer a part of the serpentlua core so i can include sol2

namespace SerpentLua::internal::ScriptBuiltin { // builtin plugin, can be included within a script by doing `--@plugins serpentlua_builtin`
	void entry(lua_State* L);

	inline SerpentLua::Plugin* plugin = nullptr;

	// things for each specific script
	struct ScriptContext {
		sol::table mainModule;
		lua_State* L;
		ScriptMetadata* metadata;
		ScriptContext() {}
	};

	inline std::unordered_map<lua_State*, ScriptContext> contexts;

	geode::Result<> initPlugin();

	sol::table logging(sol::state_view state);

	ScriptMetadata* getMetadata(lua_State* L);
};