#pragma once
#include <internal/SerpentLua.hpp>
#include <internal/ui/ScriptsLayer.hpp>

// i will really only not use this here when theres values that are in this and arent in that, like scriptmetadata::errors
#define METADATA_GET(op) (this->source == Source::Plugins ? std::get<PluginMetadata*>(this->metadata)->op : std::get<ScriptMetadata*>(this->metadata)->op)

namespace SerpentLua::internal::ui {
	class ScriptItem : public cocos2d::CCNode {
	private:
		cocos2d::extension::CCScale9Sprite* bg;
		cocos2d::CCNode* title;
		cocos2d::CCNode* mainContainer;
		cocos2d::CCLabelBMFont* titleLabel;
		cocos2d::CCLabelBMFont* versionLabel; // use the title container for the version too
		cocos2d::CCMenu* devContainer;
		cocos2d::CCLabelBMFont* dev;
		cocos2d::CCMenu* viewMenu;
		bool init(const DisplayInfo& theMetadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, Source source);
		void listener(float dt);
		Source source;
		bool lastState;
	public:
		static ScriptItem* create(const DisplayInfo& metadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, Source source);
		CCMenuItemToggler* viewBtn;
		DisplayInfo metadata;
	};
}