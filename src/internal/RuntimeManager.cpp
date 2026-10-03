#include <internal/RuntimeManager.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

RuntimeManager* RuntimeManager::get() {
	static RuntimeManager* instance;
	if (!instance) instance = new RuntimeManager();
	return instance;
}

Result<SerpentLua::Plugin*, std::string> RuntimeManager::getLoadedPluginByID(const std::string& id) {
	if (!loadedPlugins.contains(id)) {
		return Err("Plugin Getter: Loaded plugin {} does not exist.", id);
	}
	return Ok(loadedPlugins[id]);
}

Result<SerpentLua::PluginMetadata*, std::string> RuntimeManager::getPluginByID(const std::string& id) {
	if (!plugins.contains(id)) return Err("Plugin Getter: Plugin {} does not exist", id);
	return Ok(plugins[id]);
}

void RuntimeManager::setLoadedScript(Script* script) {
	loadedScripts.insert({script->getMetadata()->getID(), script});
}

void RuntimeManager::setScript(ScriptMetadata* script) {
	scripts.insert({script->getID(), script});
}

void RuntimeManager::setPlugin(Plugin* plugin) {
	auto id = plugin->metadata->getID();
	plugins.insert({id, plugin->metadata});
	loadedPlugins.insert({id, plugin});
}

Result<Script*, std::string> RuntimeManager::getLoadedScriptByID(const std::string& id) {
	if (!loadedScripts.contains(id)) {
		if (scripts.contains(id)) return Err("Loaded Script Getter: Cannot retrieve script {} as it failed exeuction.", id);
		return Err("Script Getter: Script {} does not exist.", id);
	}
	return Ok(loadedScripts[id]);
}

Result<SerpentLua::ScriptMetadata*, std::string> RuntimeManager::getScriptByID(const std::string& id) {
	if (!scripts.contains(id)) return Err("Script Getter: Script {} does not exist.", id);
	return Ok(scripts[id]);
}

Script* RuntimeManager::getLoadedScriptByState(lua_State* L) {
	for (const auto [k, v] : RuntimeManager::get()->getAllLoadedScripts()) {
		if (v->getLuaState() != L) continue;

		return v;
	}

	return nullptr;
}

SerpentLua::ScriptMetadata* RuntimeManager::getScriptByState(lua_State* L) {
	for (const auto [k, v] : RuntimeManager::get()->getAllLoadedScripts()) {
		if (v->getLuaState() != L) continue;

		return v->getMetadata();
	}

	return nullptr;
}

std::map<std::string, Script*> RuntimeManager::getAllLoadedScripts() {
	return loadedScripts;
}

std::map<std::string, SerpentLua::ScriptMetadata*> RuntimeManager::getAllScripts() {
	return scripts;
}

std::map<std::string, SerpentLua::Plugin*> RuntimeManager::getAllLoadedPlugins() {
	return loadedPlugins;
}

std::map<std::string, SerpentLua::PluginMetadata*> RuntimeManager::getAllPlugins() {
	return plugins;
}

std::map<std::string, std::vector<std::string>> RuntimeManager::getAllScriptErrors() {
	return scriptErrors;
}

std::vector<std::string> RuntimeManager::getScriptErrors(const std::string& id) {
	return scriptErrors[id];
}

void RuntimeManager::addScriptError(const std::string& id, std::string error) {
	scriptErrors[id].push_back(error);
}

void RuntimeManager::removeLoadedScript(Script* script) {
	auto id = script->getMetadata()->getID();
	if (loadedPlugins.contains(script->getMetadata()->getID())) loadedPlugins.erase(id);
	if (script->getLuaState()) lua_close(script->getLuaState());
	delete script;
}

void RuntimeManager::removeLoadedPlugin(Plugin* plugin) {
	auto id = plugin->metadata->getID();
	if (loadedPlugins.contains(id)) loadedPlugins.erase(id);
	delete plugin;
}