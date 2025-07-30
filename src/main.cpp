#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <filesystem>
#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal;
using namespace SerpentLua;

Result<void, std::vector<std::pair<std::string, std::string>>> createDirs(const std::filesystem::path& where, const std::vector<std::string>& dirs) {
	std::vector<std::pair<std::string, std::string>> errs;
	for (auto& dir : dirs) {
		if (!std::filesystem::exists(where / dir)) {
			log::info("Directory {} does not exist, creating it now.", dir);
			auto createDirRes = utils::file::createDirectory(where / dir);
			if (createDirRes.isErr()) errs.push_back({dir, createDirRes.err().value()});
		}
	}

	if (errs.empty()) return Ok();

	return Err(errs);
}

$on_mod(Loaded) {
	log::info("SerpentLua loaded!");
	log::info("Running Lua version: {}", LUA_VERSION);
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	auto configDir = Mod::get()->getConfigDir();
	
	auto res = createDirs(configDir, {"plugin_global_deps", "plugin_deps", "plugins", "scripts"});
	if (res.isErr()) {
		for (auto& err : res.err().value()) {
			log::error("Creating directory {} failed: {}", err.first, err.second);
		}
		return;
	}
	ScriptBuiltin::initPlugin();

	// initialize all the native plugins! (mod plugins are initialized by the mods themselves)

	for (const auto& file : std::filesystem::directory_iterator(configDir/"plugins")) {
		if (file.path().extension().string() != ".dll") {
			log::warn("Found non-dll file in plugins directory, will be ignored.");
			continue;
		}

		auto pluginRes = SerpentLua::Plugin::createNative(file.path());
		if (pluginRes.isErr()) {
			log::error("{}", pluginRes.err().value());
			continue;
		}

		pluginRes.unwrap()->setPlugin();
	}

	// and now for the scripts!

	// setup metadata first
	for (const auto& file : std::filesystem::directory_iterator(configDir/"scripts")) {
		if (file.path().extension().string() != ".lua") {
			log::warn("Non-lua file was found in scripts directory, will be ignored.");
			continue;
		}

		auto res = ScriptMetadata::createFromScript(file);
		if (res.isErr()) {
			log::error("{}", res.err());
			continue;
		}
		RuntimeManager::get()->setScript(res.unwrap());
	}

	// TODO: implement script enabling and disabling
	// assume all scripts are enabled until i do the thing
	for (auto& pair : RuntimeManager::get()->getAllScripts()) {
		auto res = script::create(pair.second);
		if (res.isErr()) {
			log::error("{}", res.err().value());
			continue;
		}
		auto script = res.unwrap();
		
		RuntimeManager::get()->setLoadedScript(script);

		auto loadres = script->loadPlugins();

		if (loadres.isErr()) {
			log::error("{}", loadres.err().value());
		}

		auto execres = script->execute();
		if (execres.isErr()) {
			log::error("{}", execres.err().value());
			continue;
		}
	}
}

/*
class $modify(MenuLayerHook, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		return true;
	}
	
	void onMoreGames(CCObject*) {
		ScriptBuiltin::initPlugin();
		auto metadataRes = ScriptMetadata::createFromScript(Mod::get()->getConfigDir() / "scripts" / "yellowcat98_test.lua");
		if (metadataRes.isErr()) {
			log::error("Metadata err: {}", metadataRes.err().value());
			return;
		}
		auto metadata = metadataRes.unwrap();
		auto res = script::create(metadata);
		if (res.isErr()) {
			log::error("Create err: {}", res.err().value());
			return;
		}

		auto script = res.unwrap();

		ScriptBuiltin::plugin->getEntry()(script->getLuaState());

		RuntimeManager::get()->setScript(metadata);
		RuntimeManager::get()->setLoadedScript(script);
		auto execres = script->execute();
		if (execres.isErr()) {
			log::error("exec Err: {}", execres.err().value());
		}
	}
	

	void onMoreGames(CCObject*) {
		
		auto metadataRes = ScriptMetadata::createFromScript(Mod::get()->getConfigDir() / "scripts" / "yellowcat98_test.lua");
		if (metadataRes.isErr()) {
			log::error("Metadata err: {}", metadataRes.err().value());
			return;
		}
		auto metadata = metadataRes.unwrap();
		auto res = script::create(metadata);
		if (res.isErr()) {
			log::error("Create err: {}", res.err().value());
			return;
		}

		auto script = res.unwrap();

		RuntimeManager::get()->setLoadedScript(script);

		auto pluginRes = script->loadPlugins();
		if (pluginRes.isErr()) {
			log::error("{}", pluginRes.err().value());
			return;
		}
		auto execRes = script->execute();
		if (execRes.isErr()) {
			log::error("{}", execRes.err());
		}
	}
};
*/