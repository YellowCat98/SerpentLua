#pragma once

#include <SerpentLua.hpp>

namespace SerpentLua::internal {
	class Script {
	public:
		static geode::Result<Script*, std::string> create(ScriptMetadata* metadata);
		static geode::Result<Script*, std::string> getLoadedScript(const std::string& id);
		ScriptMetadata* getMetadata();
		lua_State* getLuaState();
		lua_State* createState();
		geode::Result<> execute(); // Executes THE SCRIPT.
		geode::Result<> loadPlugins();
	private: 
		ScriptMetadata* metadata;
		lua_State* state; // each script requires its own lua state for guaranteed isolation. (and also so you can get a script by just a function call!)
		std::vector<Plugin*> pendingPlugins; // this only exists so a failed script doesnt keep a plugin in use
		bool executed;
		bool pluginsInitiated;
		void terminate(); // currently only used to terminate scripts that failed execution.
		void commitLoadedPlugins();
	};
}