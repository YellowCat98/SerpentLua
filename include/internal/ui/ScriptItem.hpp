#pragma once
#include <internal/SerpentLua.hpp>
#include <internal/ui/ScriptsLayer.hpp>

// i will really only not use this here when theres values that are in this and arent in that, like scriptmetadata::errors
#define METADATA_GET(op) (plugin ? std::get<PluginMetadata*>(this->metadata)->op : std::get<ScriptMetadata*>(this->metadata)->op)

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
		bool init(void* theMetadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, bool plugin);
		void listener(float dt);
		bool plugin;
	public:
		static ScriptItem* create(void* metadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, bool plugin);
		CCMenuItemToggler* viewBtn;
		std::variant<ScriptMetadata*, PluginMetadata*> metadata; // nvm this was the one that was needed to access within ScriptsLayer!
	};
}