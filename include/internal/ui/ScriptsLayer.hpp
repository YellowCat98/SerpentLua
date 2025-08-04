#pragma once
#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
    class ScriptsLayer : public cocos2d::CCLayer {
	public:
		static ScriptsLayer* create();
		static cocos2d::CCScene* scene();
	private:
		bool init();
		geode::ScrollLayer* scroll;
		geode::Border* border;
		cocos2d::CCSize winSize;
		void keyBackClicked() override;
	};
}