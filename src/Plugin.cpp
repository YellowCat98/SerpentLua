#include <SerpentLua.hpp>
#include <internal/RuntimeManager.hpp>
#include <internal/Utility.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

std::function<void(lua_State*)> Plugin::getEntry() {
	return entry;
}

void Plugin::setPlugin() {
	return internal::RuntimeManager::get()->setPlugin(this);
}

Result<Plugin*, std::string> Plugin::create(PluginMetadata* metadata, std::function<void(lua_State*)> entry) {
	log::info("Plugin {}: initialized.", metadata->getID());
	auto ret = new (std::nothrow) Plugin();
	if (!ret) return Err("Plugin {}: Not enough memory to create plugin.", metadata->getID());
	ret->loadCount = 0;

	ret->metadata = metadata;
	ret->entry = entry;

	return Ok(ret);
}