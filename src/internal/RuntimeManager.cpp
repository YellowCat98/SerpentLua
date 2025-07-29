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
        if (plugins.contains(id)) return Err("Loaded Plugin Getter: Cannot retrieve plugin {} as it did not load.", id);
        return Err("Script Getter: Script {} does not exist.", id);
    }
    return Ok(loadedPlugins[id]);
}

Result<SerpentLua::PluginMetadata*, std::string> RuntimeManager::getPluginByID(const std::string& id) {
    if (!plugins.contains(id)) return Err("Plugin Getter: Plugin {} does not exist.", id);
    return Ok(plugins[id]);
}

void RuntimeManager::setLoadedScript(script* script) {
    loadedScripts.insert({script->getMetadata()->id, script});
}

void RuntimeManager::setScript(ScriptMetadata* script) {
    scripts.insert({script->id, script});
}

Result<script*, std::string> RuntimeManager::getLoadedScriptByID(const std::string& id) {
    if (!loadedScripts.contains(id)) {
        if (scripts.contains(id)) return Err("Loaded Script Getter: Cannot retrieve script {} as it failed exeuction.", id);
        return Err("Script Getter: Script {} does not exist.", id);
    }
    return Ok(loadedScripts[id]);
}

Result<ScriptMetadata*, std::string> RuntimeManager::getScriptByID(const std::string& id) {
    if (!scripts.contains(id)) return Err("Script Getter: Script {} does not exist.", id);
    return Ok(scripts[id]);
}

std::map<std::string, script*> RuntimeManager::getAllLoadedScripts() {
    return loadedScripts;
}