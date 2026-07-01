#pragma once

#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

// i couldve just derived likeitempopup but eh
namespace SerpentLua::internal::ui {
	class SetUserStatusPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		CCMenuItemSpriteExtra* left;
		CCMenuItemSpriteExtra* right;
		cocos2d::CCLabelBMFont* status;
		cocos2d::CCMenu* actionsMenu;
		CCMenuItemSpriteExtra* apply;
		GJUserScore* score;
		geode::TextInput* textInput;
		int currentPage;
		int currentIndex;
		bool init(GJUserScore* score);
		void startOperation(cocos2d::CCObject*);
		void movePage(cocos2d::CCObject*);
		void loadPage(int page);
		std::vector<ServerManager::Status> statuses;
		ServerManager::Status currentStatus;
	public:
		static SetUserStatusPopup* create(GJUserScore* score);
	};
};