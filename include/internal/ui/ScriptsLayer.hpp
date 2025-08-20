#pragma once
#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
    class ScriptsLayer : public cocos2d::CCLayer {
	public:
		static ScriptsLayer* create(bool plugin); // whether to make it show plugins or show scripts
		static cocos2d::CCScene* scene(bool plugin);
	private:

		void keyBackClicked() override;

		bool init(bool plugin);

		void setupScriptsList();
		void setupPluginsList();

		void loadPage(int page);
		void refreshWith(cocos2d::CCArray* array);

		GJListLayer* m_scriptsListLayer;
		
		CCMenuItemSpriteExtra* nextBtn;
		CCMenuItemSpriteExtra* backBtn;
		// these buttons are needed here so that we can change their opacity in loadPage
		void callbackMovePage(cocos2d::CCObject*);

		int currentPage;
		int itemsPerPage;

		cocos2d::CCSize winSize;
		std::map<std::string, ScriptMetadata*> scripts; // for convenience, PluginMetadata is easily convertible to ScriptMetadata

		bool plugin;
	};
}