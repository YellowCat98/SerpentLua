#pragma once
#include <Geode/Geode.hpp>
#include <Geode/binding/SetIDPopupDelegate.hpp>
#include <internal/SerpentLua.hpp>


namespace SerpentLua::internal::ui {
	// this takes into account always on top members of std::map<std::string, T>

	enum class Source {
		Scripts,
		Plugins,
		Index
	};

	// this only contains what scriptitem needs
	struct DisplayInfo {
		static DisplayInfo create(matjson::Value& metadata);
		static DisplayInfo createFromScript(void* script, bool isScript);

		std::string name;
		std::string developer;
		std::string id;
		std::string version;
		std::string serpentVersion;

		std::string description;
		std::string downloadLink;
		std::string scriptExample;

		std::string sourceCode;
		std::string source;

		int downloadCount = 0;
		int accountId = 0;

		int64_t releaseDate = 0;
		int64_t lastUpdateDate = 0;

		bool featured;

		std::string path;
		bool native;
		bool loaded;
		bool script;

		std::variant<ScriptMetadata*, PluginMetadata*> internal; // represents the ScriptMetadata or PluginMetadata, nullptr if index.
	};

	template <typename T> T sourceValue(Source source, T script, T plugin, T index) {
		if (source == Source::Scripts) {
			return script;
		} else if (source == Source::Plugins) {
			return plugin;
		} else {
			return index;
		}
	}

	class ScriptsLayer : public cocos2d::CCLayer, public SetIDPopupDelegate {
	public:
		static ScriptsLayer* create(Source source); // whether to make it show plugins or show scripts
		static cocos2d::CCScene* scene(Source source);
		inline static bool pendingRestart = false; // making it static allows for when you enter the plugins ui the indicator is still there
		static void changesMade();
		geode::async::TaskHolder<geode::utils::web::WebResponse> serverListener;
	private:

		void keyBackClicked() override;

		bool init(Source source);

		void setupScriptsList();

		void loadPageServer(int page);
		void loadPageLocal(int page);
		void loadPage(int page); // just forwards the call to loadPageServer or loadPageLocal lol
		void refreshWith(cocos2d::CCArray* array);

		GJListLayer* m_scriptsListLayer;
		
		CCMenuItemSpriteExtra* nextBtn;
		CCMenuItemSpriteExtra* backBtn;
		// these buttons are needed here so that we can change their opacity in loadPage
		void callbackMovePage(cocos2d::CCObject*);

		cocos2d::CCMenu* infoMenu;
		cocos2d::CCLabelBMFont* infoLabel; // shows info about what page it is, how many pages, and how many items
	
		void importPlugin(cocos2d::CCObject*);

		void startImport(std::filesystem::path path, std::filesystem::path dest, std::filesystem::copy_options options);

		int currentPage;
		int itemsPerPage;
		int totalPages;
		int totalItems;

		Source source;

		cocos2d::CCSize winSize;
		std::vector<std::pair<std::string, DisplayInfo>> scripts;
		std::unordered_map<std::string, std::string> nameCache; // caches the plugins and scripts names so we can use them in the comparator without having to use runtimemanager to look up

		void pendingRestartListener(float dt);
		cocos2d::CCSprite* pendingRestartIndicator;

		geode::LoadingSpinner* spinner;

		void setIDPopupClosed(SetIDPopup* popup, int num) override;
	};
}