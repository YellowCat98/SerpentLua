#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>

namespace SerpentLua::internal::ui {
	class PluginInfoPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		bool init(const DisplayInfo& info);
	public:
		static PluginInfoPopup* create(const DisplayInfo& info);
	};
};