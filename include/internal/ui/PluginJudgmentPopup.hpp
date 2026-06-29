#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

// i couldve just derived likeitempopup but eh
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