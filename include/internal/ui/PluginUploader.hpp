#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class PluginUploader : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		bool init();
		void onUpload(cocos2d::CCObject*);
	public:
		static PluginUploader* create();
	};
};