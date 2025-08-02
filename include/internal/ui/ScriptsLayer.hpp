#pragma once
#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
    class ScriptsLayer : public geode::Popup<> {
	public:
		static ScriptsLayer* create();
	private:
		bool setup() override;
		bool initAnchored();
		geode::ScrollLayer* scroll;
		geode::Border* border;
		cocos2d::CCSize winSize;
	};
}