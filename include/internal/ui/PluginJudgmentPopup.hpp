#pragma once

#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

// i couldve just derived likeitempopup but eh
namespace SerpentLua::internal::ui {
	class PluginJudgmentPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		DisplayInfo info;
		bool featured = false;
		CCMenuItemSpriteExtra* approve;
		CCMenuItemSpriteExtra* reject;
		CCMenuItemToggler* feature;
		bool init(const DisplayInfo& info);
		void startOperation(CCObject*);
	public:
		static PluginJudgmentPopup* create(const DisplayInfo& info);
	};
};