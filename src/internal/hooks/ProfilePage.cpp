#include <internal/SerpentLua.hpp>
#include <Geode/modify/ProfilePage.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

class $modify(ProfilePage) {
	void loadPageFromUserInfo(GJUserScore* score) {
		ProfilePage::loadPageFromUserInfo(score);
		if (m_ownProfile) return;
		if (!ServerManager::get()->resolveStatus(ServerManager::Status::Staff)) return;

		auto bottomMenu = m_mainLayer->getChildByID("bottom-menu"); // i wake up everyday thankful for node ids

		auto sprite = CCSprite::create("user_status.png"_spr);
		sprite->setScale(0.75f);
		auto button = CCMenuItemExt::createSpriteExtra(sprite, [](CCMenuItemSpriteExtra*) {

		});
		button->setID("set-status-button"_spr);

		bottomMenu->addChild(button);
		bottomMenu->updateLayout();
	}
};