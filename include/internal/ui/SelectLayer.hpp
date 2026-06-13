#pragma once

#include <Geode/Geode.hpp>
#include <ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class SelectLayer : public cocos2d::CCLayer {
	private:
		void keyBackClicked();

		bool init(bool adminPanel);
		void createPeasantPanel();
		void createAdminPanel();

		cocos2d::CCMenu* buttonMenu;
		cocos2d::CCMenu* infoMenu;
	public:
		static SelectLayer* create(bool adminPanel);
		static cocos2d::CCScene* scene(bool adminPanel);
	};
};