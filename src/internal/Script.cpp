#include <SerpentLua.hpp>
#include <internal/Script.hpp>
#include <internal/RuntimeManager.hpp>
#include <internal/Utility.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;


SerpentLua::ScriptMetadata* Script::getMetadata() {
	return metadata;
}

lua_State* Script::getLuaState() {
	return state;
}

lua_State* Script::createState() {
	log::debug("Script {} state creation: Initialized.", this->metadata->getID());
	lua_State* state = luaL_newstate();

	auto openAsGlobal = [](lua_State* L, const char* name, lua_CFunction openf) {
		openf(L);
		lua_setglobal(L, name);
	};

	openAsGlobal(state, "_G", luaopen_base);
	// nil out the bad guys from _G before continuing loading!

	lua_pushnil(state); lua_setglobal(state, "dofile");
	lua_pushnil(state); lua_setglobal(state, "loadfile");
	lua_pushnil(state); lua_setglobal(state, "require");
	lua_pushnil(state); lua_setglobal(state, "load");
	lua_pushnil(state); lua_setglobal(state, "dostring");


	openAsGlobal(state, LUA_MATHLIBNAME, luaopen_math);
	openAsGlobal(state, LUA_TABLIBNAME, luaopen_table);
	openAsGlobal(state, LUA_STRLIBNAME, luaopen_string);

	lua_newtable(state);
	lua_setglobal(state, "serpentlua_modules");

	lua_pushcfunction(state, [](lua_State* L) -> int {
		const char* module = luaL_checkstring(L, 1);

		lua_getglobal(L, "serpentlua_modules");
		if (!lua_istable(L, -1)) {
			return luaL_error(L, "serpentlua_modules is not defined.");
		}

		lua_getfield(L, -1, module);

		if (lua_isnil(L, -1)) {
			return luaL_error(L, "Module %s was not found.", module);
		}

		if (lua_isfunction(L, -1)) {
			lua_pushvalue(L, -1);
			lua_call(L, 0, 1);

			lua_pushvalue(L, -1);
			lua_getglobal(L, "serpentlua_modules");

			lua_insert(L, -2);

		lua_setfield(L, -1, module);

			lua_pop(L, 1);
		}

		return 1;
	});
	lua_setglobal(state, "require");

	lua_pushlightuserdata(state, this);
	lua_setfield(state, LUA_REGISTRYINDEX, "owner_script");

	lua_atpanic(state, [](lua_State* L) -> int {
		lua_getfield(L, LUA_REGISTRYINDEX, "owner_script");
		auto* self = static_cast<Script*>(lua_touserdata(L, -1));
		lua_pop(L, 1);

		auto fancyErr = fmt::format(
			"A script has encountered an unrecoverable error and has panicked.\n"
			"=======================================\n"
			"Faulty script: {}\n\n"
			"=================Error===================\n"
			"{}",
			self->getMetadata()->getID(), lua_tostring(L, -1)
		);
		log::error("\n{}", fancyErr);
		#ifdef GEODE_IS_WINDOWS
		MessageBoxA(
			nullptr,
			fancyErr.c_str(), 
			"SerpentLua: LUA PANIC!", MB_OK | MB_ICONERROR);
		#endif
		return 0;
	});

	return state;
}

geode::Result<> Script::loadPlugins() {
	for (const auto& [pluginID, versionString] : this->metadata->getPlugins()) {
		auto pluginRes = RuntimeManager::get()->getLoadedPluginByID(pluginID);
		if (pluginRes.isErr()) {
			auto err = Err("Script `{}` plugin loading: Plugin getter returned an error:\n\n{}\n\nWill terminate for the rest of this session.", this->metadata->getID(), pluginRes.err().value());
			RuntimeManager::get()->removeLoadedScript(this);
			return err;
		}
		auto plugin = pluginRes.unwrap();

		if (versionString != "*") {
			auto versionRes = VersionInfo::parse(versionString);
			if (versionRes.isErr()) {
				auto err = Err("Script `{}` plugin loading: Plugin `{}` Version cannot be parsed", this->metadata->getID(), pluginID);
				RuntimeManager::get()->removeLoadedScript(this);
				return err;
			}
			auto version = versionRes.unwrap();

			auto pluginVersion = VersionInfo::parse(plugin->metadata->getVersion()).unwrap(); // this cant return err because we already checked when we loaded it
			if (!Utility::versionInfoCompare(version, pluginVersion)) {
				auto err = Err("Script `{}` plugin loading: The script depends on version {} for plugin {} but you have version {}", this->metadata->getID(), versionString, pluginID, plugin->metadata->getVersion());
				RuntimeManager::get()->removeLoadedScript(this);
				return err;
			}
		}

		plugin->getOnScriptLoaded()(this->getLuaState());

		pendingPlugins.push_back(plugin);
	}

	return Ok();
}

// only terminate when a script fails inital execution, it will crash if anything after initial execution fails, this is to prevent before-the-game-loads crashes!
// when i said initial execution i meant executing the main chunk
geode::Result<> Script::execute() {
	if (luaL_dofile(this->state, this->metadata->getPath().c_str()) != LUA_OK) {
		auto err = Err("Script `{}` execution: \n\n{}\n\nScript has failed initial execution, will terminate for the rest of this session.", metadata->getID(), std::string(lua_tostring(this->state, -1)));
		RuntimeManager::get()->removeLoadedScript(this);
		return err;
	}
	this->commitLoadedPlugins(); // this code is SUPPOSED to only be reached after plugins were loaded in and initial execution succeeds
	return Ok();
}

void Script::commitLoadedPlugins() {
	for (auto& plugin : pendingPlugins) {
		plugin->loadCount++;
	}
}

geode::Result<Script*, std::string> Script::getLoadedScript(const std::string& id) {
	return SerpentLua::internal::RuntimeManager::get()->getLoadedScriptByID(id);
}

geode::Result<Script*, std::string> Script::create(ScriptMetadata* metadata) {
	auto ret = new (std::nothrow) Script();
	if (!ret) return Err("Script `{}` creation: Not enough memory to create script.", metadata->getID());
	ret->metadata = metadata;

	ret->state = ret->createState();
	if (!ret->state) return Err("Script `{}` creation: Not enough memory to start Lua interpreter.", metadata->getID());

	log::debug("Script `{}` creation: Created successfully!", metadata->getID());
	return Ok(ret);
}