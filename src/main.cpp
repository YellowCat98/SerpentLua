#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <filesystem>
#include <internal/SerpentLua.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <internal/std/PluginEntry.hpp>
#include <tulip/TulipHook.hpp>

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
	
	auto res = createDirs(configDir, {"plugin_global_deps", "plugin_deps", "plugins", "scripts", "playground"});
	if (res.isErr()) {
		auto errs = *(res.err());
		for (auto& err : errs) {
			log::error("Creating directory {} failed: {}", err.first, err.second);
		}
		return;
	}
	auto initpluginres = ScriptBuiltin::initPlugin();
	if (initpluginres.isErr()) {
		log::error("{}", initpluginres.err().value());
		return;
	}

	geode::listenForSettingChanges<bool>("dev-mode", +[](bool change) {
		if (change) {
			Mod::get()->setSavedValue<bool>("should-show-warning", true); // basically, since this function gets called after the setting was set, it makes it a little wonky to add a warning here
		}
	});

	// initialize all the native plugins! (mod plugins are initialized by the mods themselves)

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

	// and now for the scripts!

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

	std::vector<std::string> theUnfortunates;

	for (const auto& [key, value] : RuntimeManager::get()->getAllLoadedPlugins()) {
		if (value->loadCount == 0) {
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

		if (!std::filesystem::exists(Mod::get()->getConfigDir()/"plugin_global_deps"/"lua.dll")) {
			auto alert = FLAlertLayer::create("Missing DLL.",
				fmt::format(
					"lua.dll was not found in the mod's config directory.\n"
					"lua.dll is essential for plugins to work.\n"
					"Scripts will load fine, however they will not be able to use plugins.\n"
					"Please install lua.dll in the following directory:\n{}",
					(Mod::get()->getConfigDir()/"plugin_global_deps"/"lua.dll").string()
				).c_str(),
				"OK"
			);

			alert->m_scene = this;
			alert->show();
		}

		if (Mod::get()->setSavedValue<bool>("should-show-warning", false)) {
			auto popup = geode::createQuickPopup("SerpentLua: Dev mode enabled!",
				"It appears that you have enabled dev mode.\n"
				"Dev mode is a setting that allows any plugin to load, regardless if it was imported through the plugin importer or not.\n"
				"This setting is meant for plugin developers.\n"
				"This setting did not apply as a safety measure.\n"
				"If you want this setting to apply, restart the game.\n"
				"Otherwise, disable this setting.",
				"Disable", "Cancel",
				[](FLAlertLayer*, bool btn2) {
					if (btn2) {
						
					} else {
						Mod::get()->setSettingValue<bool>("dev-mode", false);
					}

					geode::createQuickPopup("Restart",
						"Would you like to restart now?",
						"Restart", "Later",
						[](FLAlertLayer*, bool btn2) {
							if (!btn2) utils::game::restart(true);
						}
					);
				}, false);

			popup->m_scene = this;
			popup->show();
		}

		auto bottomMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
		bottomMenu->addChild(CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("serpentluaButton.png"_spr)), [](CCObject*) {
			CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(false)));
		}));

		bottomMenu->updateLayout();

		return true;
	}
};