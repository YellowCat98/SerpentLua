#pragma once

#include <SerpentLua.hpp>
#include <internal/Script.hpp>

namespace SerpentLua::internal {
	class RuntimeManager {
	public:
		static RuntimeManager* get();
		geode::Result<Script*, std::string> getLoadedScriptByID(const std::string& id); // Script::getLoadedScriptByID();
		geode::Result<ScriptMetadata*, std::string> getScriptByID(const std::string& id); // ScriptMetadata::getScriptByID();
		ScriptMetadata* getScriptByState(lua_State* L); // ScriptMetadata::getScriptByState();
		Script* getLoadedScriptByState(lua_State* L);

		geode::Result<Plugin*, std::string> getLoadedPluginByID(const std::string& id);
		geode::Result<PluginMetadata*, std::string> getPluginByID(const std::string& id);

		void setLoadedScript(Script* script);
		void setScript(ScriptMetadata* script);

		void setPlugin(Plugin* plugin);

		std::map<std::string, Script*> getAllLoadedScripts();
		std::map<std::string, ScriptMetadata*> getAllScripts();

		std::map<std::string, SerpentLua::Plugin*> getAllLoadedPlugins();
		std::map<std::string, SerpentLua::PluginMetadata*> getAllPlugins();

		std::map<std::string, std::vector<std::string>> getAllScriptErrors();
		std::vector<std::string> getScriptErrors(const std::string& id);
		void addScriptError(const std::string& id, std::string error);

		// these two cowboys both terminate the plugin and script
		// there might also be cases where i'd have to call this function without the plugin/script being registered at all
		void removeLoadedScript(Script* script);
		void removeLoadedPlugin(Plugin* plugin);
	private:
		// using std::map so i can retrieve a script directly through id
		std::map<std::string, ScriptMetadata*> scripts;
		std::map<std::string, Script*> loadedScripts;

		std::map<std::string, SerpentLua::PluginMetadata*> plugins;
		std::map<std::string, SerpentLua::Plugin*> loadedPlugins;

		std::map<std::string, std::vector<std::string>> scriptErrors;
	};
}