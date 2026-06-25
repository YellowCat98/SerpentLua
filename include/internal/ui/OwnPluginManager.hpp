#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class OwnPluginManager : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		geode::async::TaskHolder<std::pair<std::string, bool>> m_uploadListener;
		geode::MDTextArea* md;
		CCMenuItemSpriteExtra* nextBtn;
		CCMenuItemSpriteExtra* backBtn;
		std::vector<std::string> prettyStrings;
		int currentPage;
		geode::TextInput* repoInput;
		geode::TextInput* tagInput;
		CCMenuItemSpriteExtra* uploadBtn;
		bool init();
		std::string createPrettyPluginInfo(const DisplayInfo& info);
		void updateMD();
		void loadPage(int page);
		void movePage(cocos2d::CCObject*);
		void uploadPlugin(cocos2d::CCObject*);
	public:
		static OwnPluginManager* create();
	};
};