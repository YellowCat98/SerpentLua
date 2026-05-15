#pragma once

#include <Geode/Geode.hpp>
#include <ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class SelectLayer : public cocos2d::CCLayer {
	private:
		void keyBackClicked();

		bool init();

	public:
		static SelectLayer* create();
		static cocos2d::CCScene* scene();
	};
};