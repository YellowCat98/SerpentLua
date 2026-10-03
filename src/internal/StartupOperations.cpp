#include <internal/StartupOperations.hpp>
#include <internal/RuntimeManager.hpp>
#include <internal/Utility.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void StartupOperations::loadScripts() {
	auto configDir = Mod::get()->getConfigDir();
	// setup metadata first
	for (const auto& file : std::filesystem::directory_iterator(configDir/"scripts")) {
		if (file.path().extension() != ".lua") {
			log::warn("Non-lua file was found in scripts directory, will be ignored.");
			continue;
		}

		auto res = ScriptMetadata::createFromScript(file);
		if (res.isErr()) {
			log::error("{}", res.err().value());
			continue;
		}
		RuntimeManager::get()->setScript(res.unwrap());
	}

	for (auto& pair : RuntimeManager::get()->getAllScripts()) {
		if (Mod::get()->getSavedValue<bool>(fmt::format("enabled-{}", pair.first))) {
			auto version = VersionInfo::parse(pair.second->getSerpentVersion());
			if (version.isErr()) {
				auto err = fmt::format("Script {} could not parse serpent-version: {}", pair.first, *(version.err()));
				RuntimeManager::get()->addScriptError(pair.second->getID(), err);
				log::error("{}", err);
				continue;
			}
			if (!Utility::versionInfoCompare(version.unwrap(), Mod::get()->getVersion())) {
				auto err = fmt::format("Script {} was made for SerpentLua version {} but you are on {}", pair.first, pair.second->getSerpentVersion(), Mod::get()->getVersion().toNonVString());
				RuntimeManager::get()->addScriptError(pair.second->getID(), err);
				log::error("{}", err);
				continue; // why didnt i do this before
			}
			auto res = Script::create(pair.second);
			if (res.isErr()) {
				RuntimeManager::get()->addScriptError(pair.second->getID(), res.err().value());
				log::error("{}", res.err().value());
				continue;
			}
			auto script = res.unwrap();
			
			RuntimeManager::get()->setLoadedScript(script);

			auto loadres = script->loadPlugins();

			if (loadres.isErr()) {
				RuntimeManager::get()->addScriptError(pair.second->getID(), loadres.err().value());
				log::error("{}", loadres.err().value());
				continue;
			}

			auto execres = script->execute();
			if (execres.isErr()) {
				RuntimeManager::get()->addScriptError(pair.second->getID(), execres.err().value());
				log::error("{}", execres.err().value());
				continue;
			}
		}
	}
}

void StartupOperations::unfortunatelyDeleteTheUnfortunates() {
	std::vector<std::string> theUnfortunates;

	for (const auto& [key, value] : RuntimeManager::get()->getAllLoadedPlugins()) {
		if (value->loadCount == 0) {
			log::trace("{}", key);
			theUnfortunates.push_back(key);
		}
	}
	// the fate has been determined

	for (auto& theUnfortunate : theUnfortunates) {
		auto mdplugin = RuntimeManager::get()->getPluginByID(theUnfortunate).unwrap();
		auto plugin = RuntimeManager::get()->getLoadedPluginByID(theUnfortunate).unwrap();
		RuntimeManager::get()->removeLoadedPlugin(plugin);
		// imagine this plugin wantign to be used and then getting TERMINATED
	}
}