#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class OwnPluginManager : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		bool init();
		geode::MDTextArea* md;
		std::vector<std::string> prettyStrings;
		std::string createPrettyPluginInfo(const DisplayInfo& info);
		void updateMD();
	public:
		static OwnPluginManager* create();
	};
};