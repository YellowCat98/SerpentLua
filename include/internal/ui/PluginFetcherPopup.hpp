#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>

namespace SerpentLua::internal::ui {
	class PluginFetcherPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> listener;
		bool init();
		geode::TextInput* textInput;
		cocos2d::CCLabelBMFont* statusLabel;
	public:
		static PluginFetcherPopup* create();
	};
};