#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <internal/StartupOperations.hpp>
#include <internal/SettingsIdk.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <internal/std/PluginEntry.hpp>
#include <Geode/utils/async.hpp>
#include <arc/prelude.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal;
using namespace SerpentLua;

Result<void, std::vector<std::pair<std::string, std::string>>> createDirs(const std::filesystem::path& where, const std::vector<std::string>& dirs) {
	std::vector<std::pair<std::string, std::string>> errs;
	for (auto& dir : dirs) {
		if (!std::filesystem::exists(where / dir)) {
			auto result = utils::file::createDirectoryAll(where / dir);
			if (result.isErr()) errs.push_back({dir, result.err().value()});
		}
	}

	if (errs.empty()) return Ok();

	return Err(errs);
}

std::vector<Mod*> getDependants() {
	std::vector<Mod*> ret;
	for (const auto mod : Loader::get()->getAllMods()) {
		if (mod->depends(Mod::get()->getID())) ret.push_back(mod);
	}

	return ret;
}

$on_mod(Loaded) {
	log::info("SerpentLua loaded!");
	log::info("Running Lua version: {}", LUA_VERSION);

	(void)Mod::get()->registerCustomSettingType("open-scripts-btn", &OpenScriptsSettingV3::parse);

	auto configDir = Mod::get()->getConfigDir();
	
	auto res = createDirs(configDir, {"scripts"});
	if (res.isErr()) {
		auto errs = *(res.err());
		for (auto& err : errs) {
			log::error("Creating directory {} failed: {}", err.first, err.second);
		}
		return;
	}

	StartupOperations::installPending(false);
	StartupOperations::installPending(true);

	auto initpluginres = ScriptBuiltin::initPlugin();
	if (initpluginres.isErr()) {
		log::error("{}", initpluginres.err().value());
		return;
	}

	log::info("Populating list of plugins that are yet to load...");
	for (const auto mod : getDependants()) {
		log::trace("Adding {} to SerpentLua::globals::pluginsYetToLoad", mod->getID());
		SerpentLua::globals::pluginsYetToLoad.push_back(mod->getID());
	}

	log::info("Waiting for plugins to load...");

	async::spawn([]() -> arc::Future<> {
		while (!SerpentLua::globals::pluginsYetToLoad.empty()) {
			co_await arc::sleep(asp::Duration::fromMillis(50));
		}

		geode::queueInMainThread([]() {
			log::info("All plugins loaded!");
			
			log::info("Loading scripts...");
			SerpentLua::internal::StartupOperations::loadScripts();

			log::info("Unloading unused plugins...");
			SerpentLua::internal::StartupOperations::unfortunatelyDeleteTheUnfortunates();
		});
	});
}