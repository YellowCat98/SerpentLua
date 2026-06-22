#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class PluginInfoPopup : public geode::Popup, public UserInfoDelegate {
	protected:
		geode::async::TaskHolder<std::pair<geode::utils::web::WebResponse, std::string>> m_listener;
		bool init(const DisplayInfo& info);
		void getUserInfoFinished(GJUserScore* score) override;
		void getUserInfoFailed(int id) override;
		CCMenuItemSpriteExtra* devBtn; // only need this one button accessible everywhere
	public:
		static PluginInfoPopup* create(const DisplayInfo& info);
	};
};