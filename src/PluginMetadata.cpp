#include <SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

PluginMetadata* PluginMetadata::createFromMod(Mod* mod) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = mod->getName();
	ret->developer = mod->getDevelopers()[0];
	ret->id = mod->getID();
	ret->version = mod->getVersion().toNonVString();
	ret->serpentVersion = Mod::get()->getVersion().toNonVString();

	return ret;
}

PluginMetadata* PluginMetadata::create(std::map<std::string, std::string>& metadata) {
	auto ret = new (std::nothrow) PluginMetadata();
	if (!ret) return nullptr;
	ret->name = metadata["name"];
	ret->developer = metadata["developer"];
	ret->id = metadata["id"];
	ret->version = metadata["version"];
	ret->serpentVersion = metadata["serpent-version"];
	return ret;
}

