#include <internal/SerpentLua.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

SerpentLua::internal::RuntimeManager* SerpentLua::internal::RuntimeManager::get() {
    static SerpentLua::internal::RuntimeManager* instance;
    if (!instance) instance = new SerpentLua::internal::RuntimeManager();
    return instance;
}

#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
void SerpentLua::internal::RuntimeManager::setLoadedScript(script* script) {
    loadedScripts.insert({script->getMetadata().id, script});
}

void SerpentLua::internal::RuntimeManager::setScript(const ScriptMetadata& script) {
    scripts.insert({script.id, script});
}

#endif

Result<script*, std::string> SerpentLua::internal::RuntimeManager::getLoadedScriptByID(const std::string& id) {
    if (!loadedScripts.contains(id)) {
        if (scripts.contains(id)) return Err("Loaded Script Getter: Cannot retrieve script {} as it failed exeuction.", id);
        return Err("Script Getter: Script {} does not exist.", id);
    }
    return Ok(loadedScripts[id]);
}

Result<ScriptMetadata, std::string> SerpentLua::internal::RuntimeManager::getScriptByID(const std::string& id) {
    if (!scripts.contains(id)) return Err("Script Getter: Script {} does not exist.", id);
    return Ok(scripts[id]);
}