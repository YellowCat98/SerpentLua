#pragma once

#include <Geode/Geode.hpp>
#include <internal/SerpentLua.hpp>

namespace SerpentLua::internal::ui {
	class PluginFetcherPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> listener;
		bool init();
		geode::TextInput* textInput;
		cocos2d::CCLabelBMFont* statusLabel;
		void setStatusLabel(const std::string& text);
	public:
		static PluginFetcherPopup* create();
	};
};