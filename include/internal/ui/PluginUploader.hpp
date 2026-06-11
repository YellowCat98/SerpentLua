#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class PluginUploader : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_Listener;
		bool init();

		// i dont think neither of these functions would be small enough to just shove into a lambda with ccmenuitemext
		void onUpload(cocos2d::CCObject*);
		void onUploadExample(cocos2d::CCObject*);
		void onImportDesc(cocos2d::CCObject*);
		// unfortunately youll have to import your description.md file because i am NOT going to use imgui so you can write multiline
	public:
		static PluginUploader* create();
	};
};