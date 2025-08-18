#pragma once
#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
    class ScriptsLayer : public cocos2d::CCLayer {
	public:
		static ScriptsLayer* create();
		static cocos2d::CCScene* scene();
	private:

		void keyBackClicked() override;

		bool init();

		void setupScriptsList();

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
		std::map<std::string, ScriptMetadata*> scripts;
	};
}