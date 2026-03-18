#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <internal/SerpentLua.hpp>
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
			log::info("Directory {} does not exist, creating it now.", dir);
			auto createDirRes = utils::file::createDirectory(where / dir);
			if (createDirRes.isErr()) errs.push_back({dir, createDirRes.err().value()});
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
	SetDefaultDllDirectories(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);

	(void)Mod::get()->registerCustomSettingType("open-scripts-btn", &OpenScriptsSettingV3::parse);

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

	// No need to add native plugins and serpentlua.std to SerpentLua::globals::pluginsYetToLoad.
	SerpentLua::internal::StartupOperations::loadNativePlugins();

	// Assume every dependant of SerpentLua is a native plugin.
	log::info("Populating list of plugins that are yet to load...");
	for (const auto mod : getDependants()) {
		log::trace("Adding {} to SerpentLua::globals::pluginsYetToLoad", mod->getID());
		SerpentLua::globals::pluginsYetToLoad.push_back(mod->getID());
	}

	log::info("Waiting for non-native plugins to load...");

	async::spawn([]() -> arc::Future<> {
		while (!SerpentLua::globals::pluginsYetToLoad.empty()) {
			co_await arc::sleep(asp::Duration::fromMillis(50));
		}

		geode::queueInMainThread([]() {
			log::info("All plugins loaded!");
			
			log::info("Loading scripts...");
			SerpentLua::internal::StartupOperations::loadScripts();

			log::info("Loading unused plugins...");
			SerpentLua::internal::StartupOperations::unfortunatelyDeleteTheUnfortunates();
		});
	});

	// and now for the scripts!
	//SerpentLua::internal::StartupOperations::loadScripts();

	//log::info("Terminating unused plugins...");
	//SerpentLua::internal::StartupOperations::unfortunatelyDeleteTheUnfortunates();
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
					if (!btn2) {
						Mod::get()->setSettingValue<bool>("dev-mode", false);
					}

					geode::createQuickPopup("Restart",
						"Would you like to restart now?",
						"Later", "Restart",
						[](FLAlertLayer*, bool btn2) {
							if (btn2) utils::game::restart(true);
						}
					);
				}, false);

			popup->m_scene = this;
			popup->show();
		}

		auto bottomMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
		bottomMenu->addChild(CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("serpentluaButton.png"_spr), CircleBaseColor::Green, CircleBaseSize::MediumAlt), [](CCObject*) {
			CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(false)));
		}));

		bottomMenu->updateLayout();

		return true;
	}
};