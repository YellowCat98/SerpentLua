#pragma once
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
	class ScriptItem : public cocos2d::CCNode {
	private:
		cocos2d::extension::CCScale9Sprite* bg;
		cocos2d::CCNode* title;
		cocos2d::CCNode* mainContainer;
		cocos2d::CCLabelBMFont* titleLabel;
		cocos2d::CCMenu* devContainer;
		cocos2d::CCLabelBMFont* dev;
		cocos2d::CCMenu* viewMenu;
		bool init(SerpentLua::ScriptMetadata* metadata, std::function<void(cocos2d::CCObject*)> onButton, const cocos2d::CCSize& size);
		void listener(float dt);
	public:
		static ScriptItem* create(SerpentLua::ScriptMetadata* metadata, std::function<void(cocos2d::CCObject*)> onButton, const cocos2d::CCSize& size);
		CCMenuItemToggler* viewBtn; // pretty much need to access it within ScriptsLayer!
	};
}