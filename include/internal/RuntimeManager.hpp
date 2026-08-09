#pragma once

#include <SerpentLua.hpp>
#include <internal/script.hpp>

namespace SerpentLua::internal {
	class RuntimeManager {
	public:
		static RuntimeManager* get();
		geode::Result<script*, std::string> getLoadedScriptByID(const std::string& id); // script::getLoadedScriptByID();
		geode::Result<ScriptMetadata*, std::string> getScriptByID(const std::string& id); // ScriptMetadata::getScriptByID();
		ScriptMetadata* getScriptByState(lua_State* L); // ScriptMetadata::getScriptByState();
		script* getLoadedScriptByState(lua_State* L);

		geode::Result<Plugin*, std::string> getLoadedPluginByID(const std::string& id);
		geode::Result<PluginMetadata*, std::string> getPluginByID(const std::string& id);

		void setLoadedScript(script* script);
		void setScript(ScriptMetadata* script);

		void setPlugin(Plugin* plugin);

		std::map<std::string, script*> getAllLoadedScripts();
		std::map<std::string, ScriptMetadata*> getAllScripts();

		std::map<std::string, SerpentLua::Plugin*> getAllLoadedPlugins();
		std::map<std::string, SerpentLua::PluginMetadata*> getAllPlugins();

		geode::Result<> removeLoadedScript(const std::string& id);

		geode::Result<> removeLoadedPlugin(const std::string& id);
	private:
		// using std::map so i can retrieve a script directly through id
		std::map<std::string, ScriptMetadata*> scripts;
		std::map<std::string, script*> loadedScripts;

		std::map<std::string, SerpentLua::PluginMetadata*> plugins;
		std::map<std::string, SerpentLua::Plugin*> loadedPlugins;
	};
}