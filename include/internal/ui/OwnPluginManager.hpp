#pragma once

#include <Geode/Geode.hpp>
#include <internal/ui/ScriptsLayer.hpp>

namespace SerpentLua::internal::ui {
	class OwnPluginManager : public geode::Popup {
	protected:
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_listener;
		geode::async::TaskHolder<geode::Result<matjson::Value>> m_beginListener;
		geode::async::TaskHolder<geode::utils::web::WebResponse> m_bodyListener;
		geode::async::TaskHolder<geode::Result<std::pair<PluginMetadata*, geode::utils::web::WebResponse>>> m_pluginMetaListener;
		geode::async::TaskHolder<geode::Result<std::string>> m_scriptHashListener;
		bool pluginMetaFinished;
		bool scriptHashFinished;

		geode::MDTextArea* md;
		CCMenuItemSpriteExtra* nextBtn;
		CCMenuItemSpriteExtra* backBtn;
		geode::TextInput* repoInput;
		geode::TextInput* tagInput;
		CCMenuItemSpriteExtra* uploadBtn;
		cocos2d::CCLabelBMFont* statusLabel;

		int currentPage;
		std::vector<std::string> prettyStrings;
		matjson::Value body;

		bool init();

		std::string createPrettyPluginInfo(const DisplayInfo& info);
		void updateMD();

		void loadPage(int page);
		void movePage(cocos2d::CCObject*);

		std::string constructUrl(std::string repo, std::string tag, std::string filename);

		// pretty much steps, setDownloadLink calls setDownloadHash, setDownloadHash calls setDescription, setDescription calls setBasicMetadata, and finally setBasicMetadata calls updateOrUpload
		void beginUpload(cocos2d::CCObject*);
		void setDownloadLinks(const matjson::Value& indexJson, const std::string& repo, const std::string& tag);
		void setDescription(const matjson::Value& indexJson, const std::string& repo, const std::string& tag);
		void setBasicMetadata(const matjson::Value& indexJson, const std::string& repo, const std::string& tag); // this gets the download hashes
		void uploadOrUpdate(const matjson::Value& indexJson, const std::string& repo, const std::string& tag); // by the time this gets called `body` should already have all the data

		void setStatusLabel(const std::string& text);
	public:
		static OwnPluginManager* create();
	};
};