#pragma once
#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>


namespace SerpentLua::internal::ui {
	// this takes into account always on top members of std::map<std::string, T>
	struct MapOrder {
		bool plugin;

		MapOrder() = default;
		MapOrder(bool plugin) : plugin(plugin) {}

		bool operator()(const std::string& a, const std::string& b) const {
			std::string newA;
			std::string newB;

			if (plugin) {
				if (a == "serpentlua.std") return true;
				if (b == "serpentlua.std") return false;

				newA = SerpentLua::internal::RuntimeManager::get()->getPluginByID(a).unwrap()->name; // it is safe to do unwrap without error checking because the map using this is populated using getAllPlugins and getAllScripts
				newB = SerpentLua::internal::RuntimeManager::get()->getPluginByID(b).unwrap()->name;
			} else {
				newA = SerpentLua::internal::RuntimeManager::get()->getScriptByID(a).unwrap()->name;
				newB = SerpentLua::internal::RuntimeManager::get()->getScriptByID(b).unwrap()->name;
			}

			return newA < newB;
		}
	};

	class ScriptsLayer : public cocos2d::CCLayer {
	public:
		static ScriptsLayer* create(bool plugin); // whether to make it show plugins or show scripts
		static cocos2d::CCScene* scene(bool plugin);
		inline static bool pendingRestart = false; // making it static allows for when you enter the plugins ui the indicator is still there
		static void changesMade();
	private:

		void keyBackClicked() override;

		bool init(bool plugin);

		void setupScriptsList();

		void loadPage(int page);
		void refreshWith(cocos2d::CCArray* array);

		GJListLayer* m_scriptsListLayer;
		
		CCMenuItemSpriteExtra* nextBtn;
		CCMenuItemSpriteExtra* backBtn;
		// these buttons are needed here so that we can change their opacity in loadPage
		void callbackMovePage(cocos2d::CCObject*);

		cocos2d::CCLabelBMFont* infoLabel; // shows info about what page it is, how many pages, and how many items

		
		void importPlugin(cocos2d::CCObject*);

		void startImport(std::filesystem::path path, std::filesystem::path dest, std::filesystem::copy_options options);

		int currentPage;
		int itemsPerPage;

		bool plugin;

		cocos2d::CCSize winSize;
		std::map<std::string, void*, MapOrder> scripts; // it appears that it was a pain in the ass to use templates here, so im using a raw pointer and then converting it to either pluginmetadata or scriptmetadata! all of this just to avoid copying

		

		void pendingRestartListener(float dt);
		cocos2d::CCSprite* pendingRestartIndicator;
	};
}