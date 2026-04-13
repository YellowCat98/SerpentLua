#include <internal/SerpentLua.hpp>

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

void RuntimeManager::setLoadedScript(script* script) {
	loadedScripts.insert({script->getMetadata()->id, script});
}

void RuntimeManager::setScript(ScriptMetadata* script) {
	scripts.insert({script->id, script});
}

void RuntimeManager::setPlugin(Plugin* plugin) {
	plugins.insert({plugin->metadata->id, plugin->metadata});
	loadedPlugins.insert({plugin->metadata->id, plugin});
}

Result<script*, std::string> RuntimeManager::getLoadedScriptByID(const std::string& id) {
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

script* RuntimeManager::getLoadedScriptByState(lua_State* L) {
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

std::map<std::string, script*> RuntimeManager::getAllLoadedScripts() {
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

Result<> RuntimeManager::removeLoadedScript(const std::string& id) {
	if (!loadedScripts.contains(id)) return Err("Loaded Script remover: The unfortunate script was not found.");
	loadedScripts.erase(id);

	return Ok();
}

Result<> RuntimeManager::removeLoadedPlugin(const std::string& id) {
	if (!loadedPlugins.contains(id)) return Err("Loaded Plugin remover: The unfortunate plugin was not found.");
	loadedPlugins.erase(id);

	return Ok();
}