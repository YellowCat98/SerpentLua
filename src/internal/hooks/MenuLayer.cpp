#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <internal/ServerManager.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/SelectLayer.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal;
using namespace SerpentLua;

class $modify(SerpentLuaMenuLayer, MenuLayer) {
	bool init() {
		if (!MenuLayer::init()) return false;

		static bool shouldAuthenticate = Mod::get()->getSettingValue<bool>("should-authenticate");
		if (shouldAuthenticate) {
			ServerManager::get()->authenticate(argon::getGameAccountData());
			shouldAuthenticate = false;
		}

		auto bottomMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
		auto btn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("serpentluaButton.png"_spr), CircleBaseColor::Green, CircleBaseSize::MediumAlt), [](CCObject*) {
			CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::SelectLayer::scene()));
		});
		btn->setID("serpentlua-btn"_spr);
		bottomMenu->addChild(btn);

		bottomMenu->updateLayout();

		return true;
	}
};