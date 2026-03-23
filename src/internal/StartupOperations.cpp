#include <internal/SerpentLua.hpp>

using namespace geode::prelude;

void SerpentLua::internal::StartupOperations::loadNativePlugins() {
	auto configDir = Mod::get()->getConfigDir();
	for (const auto& file : std::filesystem::directory_iterator(configDir/"plugins")) {

		if (file.path().extension().string() == ".dll") {
			log::error("All plugins must have the .slp extension, DLLs will not load.");
		}

		if (file.path().extension().string() != ".slp") {
			log::warn("Found non-slp file in plugins directory, will be ignored.");
			continue;
		}

		auto pluginRes = SerpentLua::Plugin::createNative(file.path());
		if (pluginRes.isErr()) {
			log::error("{}", pluginRes.err().value());
			continue;
		}

		auto unwrapped = pluginRes.unwrap();
		if (unwrapped->metadata->serpentVersion != Mod::get()->getVersion().toNonVString()) {
			log::error("Plugin {} was made for serpent version {} but you are on {}", unwrapped->metadata->id, unwrapped->metadata->serpentVersion, Mod::get()->getVersion().toNonVString());
			continue;
		}

		pluginRes.unwrap()->setPlugin();
	}
}

void SerpentLua::internal::StartupOperations::loadScripts() {
	auto configDir = Mod::get()->getConfigDir();
	// setup metadata first
	for (const auto& file : std::filesystem::directory_iterator(configDir/"scripts")) {
		if (file.path().extension().string() != ".lua") {
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
			if (pair.second->serpentVersion != Mod::get()->getVersion().toNonVString()) {
				auto err = fmt::format("Script {} was made for serpent version {} but you are on {}", pair.first, pair.second->serpentVersion, Mod::get()->getVersion().toNonVString());
				pair.second->errors.push_back(err);
				log::warn("{}", err);
				continue; // why didnt i do this before
			}
			auto res = script::create(pair.second);
			if (res.isErr()) {
				pair.second->errors.push_back(res.err().value());
				log::error("{}", res.err().value());
				continue;
			}
			auto script = res.unwrap();
			
			RuntimeManager::get()->setLoadedScript(script);

			auto loadres = script->loadPlugins();

			if (loadres.isErr()) {
				pair.second->errors.push_back(loadres.err().value());
				log::error("{}", loadres.err().value());
				continue;
			}

			auto execres = script->execute();
			if (execres.isErr()) {
				pair.second->errors.push_back(execres.err().value());
				log::error("{}", execres.err().value());
				continue;
			}
		}
	}
}

void SerpentLua::internal::StartupOperations::unfortunatelyDeleteTheUnfortunates() {
	std::vector<std::string> theUnfortunates;

	for (const auto& [key, value] : RuntimeManager::get()->getAllLoadedPlugins()) {
		if (value->loadCount == 0) {
			log::trace("{}", key);
			theUnfortunates.push_back(key);
		}
	}
	// the fate has been determined

	for (auto& theUnfortunate : theUnfortunates) {
		RuntimeManager::get()->getPluginByID(theUnfortunate).unwrap()->loaded = false;
		RuntimeManager::get()->getLoadedPluginByID(theUnfortunate).unwrap()->terminate();
		// imagine this plugin wantign to be used and then getting TERMINATED
	}
}