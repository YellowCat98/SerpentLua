#include <internal/DisplayInfo.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

DisplayInfo DisplayInfo::createFromScript(void* script, bool isScript) {
	DisplayInfo info;

	if (isScript) {
		auto theScript = static_cast<ScriptMetadata*>(script);

		info.name = theScript->getName();
		info.id = theScript->getID();
		info.developer = theScript->getDeveloper();
		info.version = theScript->getVersion();
		info.serpentVersion = theScript->getSerpentVersion();
		info.path = theScript->getPath();

		info.internal = theScript;
	} else {
		auto thePlugin = static_cast<PluginMetadata*>(script);

		info.name = thePlugin->getName();
		info.id = thePlugin->getID();
		info.developer = thePlugin->getDeveloper();
		info.version = thePlugin->getVersion();
		info.serpentVersion = thePlugin->getSerpentVersion();

		info.loaded = thePlugin->isLoaded();

		info.internal = thePlugin;
	}

	info.script = isScript;

	return info;
}