#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <filesystem>
#include <internal/SerpentLua.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <random>

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
	auto initpluginres = ScriptBuiltin::initPlugin();
	if (initpluginres.isErr()) {
		log::error("{}", initpluginres.err().value());
		return;
	}

	geode::listenForSettingChanges("dev-mode", +[](bool change) {
		if (change) {
			geode::createQuickPopup(
				"WARNING!",
				"Enabling dev mode bypasses Plugin validation!"
				"\nThis allows ALL plugins found in the plugins directory of this mod to be loaded."
				"\nThis option is only meant for developers."
				"\nEnable at your own risk.",
				"Cancel", "Proceed", // this is a reference to the hit game Deltarune.
				[](FLAlertLayer*, bool btn2) {
					if (!btn2) {
						log::info("btn2");
						Mod::get()->setSettingValue<bool>("dev-mode", false);
					}
				}
			);
		}
	});

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
		if (file.path().extension().string() != ".lua" && file.path().extension().string() != ".luau" && file.path().extension().string() != ".luac") {
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

	std::vector<std::string> theUnfortunates;

	for (const auto& [key, value] : RuntimeManager::get()->getAllLoadedPlugins()) {
		if (!value->loadedSomewhere) {
			log::debug("THE UNFORTUNATE: {}", key);
			theUnfortunates.push_back(key);
		}
	}
	// the fate has been determined

	for (auto& theUnfortunate : theUnfortunates) {
		RuntimeManager::get()->getLoadedPluginByID(theUnfortunate).unwrap()->terminate();
		// imagine this plugin wantign to be used and then getting TERMINATED
	}
}


class $modify(MenuLayerHook, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		auto bottomMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
		bottomMenu->addChild(CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("serpentluaButton.png"_spr)), [](CCObject*) {
			CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene()));
		}));

		bottomMenu->updateLayout();

		return true;
	}

	void onMoreGames(CCObject* sender) {
		auto scene = CCScene::create();
		
		CCDirector::get()->replaceScene(scene);
	}
};