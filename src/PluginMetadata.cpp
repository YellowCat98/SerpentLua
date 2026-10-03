#include <SerpentLua.hpp>
#include <internal/Utility.hpp>

using namespace geode::prelude;
using namespace SerpentLua;
using namespace SerpentLua::internal;

std::string PluginMetadata::getName() {
	return name;
}

std::string PluginMetadata::getDeveloper() {
	return developer;
}

std::string PluginMetadata::getID() {
	return id;
}

std::string PluginMetadata::getVersion() {
	return version;
}

std::string PluginMetadata::getSerpentVersion() {
	return serpentVersion;
}

bool PluginMetadata::isLoaded() {
	return loaded;
}

PluginMetadata* PluginMetadata::createFromMod(Mod* mod) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = mod->getName();
	ret->developer = mod->getDevelopers()[0];
	ret->id = mod->getID();
	ret->version = mod->getVersion().toVString();
	ret->serpentVersion = Mod::get()->getVersion().toNonVString();

	return ret;
}

PluginMetadata* PluginMetadata::create(std::map<std::string, std::string> metadata) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = metadata["name"];
	ret->developer = metadata["developer"];
	ret->id = metadata["id"];
	ret->version = metadata["version"];
	ret->serpentVersion = metadata["serpent-version"];
	return ret;
}

