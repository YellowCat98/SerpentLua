#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <filesystem>
#include <internal/SerpentLua.hpp>

using namespace geode::prelude;

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
/*
	for (auto& file : std::filesystem::directory_iterator(configDir / "scripts")) {
		if (file.is_directory()) log::warn("Found folder in scripts directory.");
		if (file.path().extension() != ".lua") log::warn("Found non-lua file in scripts directory.");

		auto script = SerpentLua::script::create(file.path());
		if (script.isErr()) {
			log::error("{}", script.err().value());
		} else {
			SerpentLua::scripts.push_back(script.unwrap());
		}
	}
*/
	
}


class $modify(MenuLayerHook, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;
		return true;
	}
/*
	void onMoreGames(CCObject*) {
		auto test = SerpentLua::Plugin::createNative(Mod::get()->getConfigDir() / "plugins" / "yellowcat98_plugintest.dll");
		auto script = SerpentLua::script::create(Mod::get()->getConfigDir() / "scripts" / "test.lua");
		if (script.isErr()) {
			log::error("{}", script.err());
			return;
		}
		if (test.isErr()) {
			log::error("{}", test.err().value());
			return;
		}
		auto scriptunwrap = script.unwrap();
		test.unwrap()->getEntry()(scriptunwrap->getLuaState());
		auto exec = scriptunwrap->execute();
		if (exec.isErr()) {
			log::error("{}", exec.err());
			return;
		}
	}
*/
};