#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class PluginJudgmentPopup : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> listener;
		bool init(const DisplayInfo& info);
		void startOperation();
	public:
		static PluginJudgmentPopup* create(const DisplayInfo& info);
	};
};