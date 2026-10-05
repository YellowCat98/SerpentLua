#pragma once
#include <string>
#include <vector>
#include <matjson.hpp>
#include <Geode/Geode.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <lua.hpp>

#ifdef GEODE_IS_WINDOWS
	#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
		#define SERPENTLUA_DLL __declspec(dllexport)
	#else
		#define SERPENTLUA_DLL __declspec(dllimport)
	#endif
#else
	#define SERPENTLUA_DLL __attribute__((visibility("default")))
#endif

namespace SerpentLua {
	struct globals {
		inline static std::vector<std::string> SERPENTLUA_DLL pluginsYetToLoad;
	};

	struct SERPENTLUA_DLL PluginMetadata final {
	public:
		static PluginMetadata* create(std::map<std::string, std::string> metadata);
		static PluginMetadata* createFromMod(geode::Mod* mod);

		std::string getName();
		std::string getDeveloper();
		std::string getID();
		std::string getVersion();
		std::string getSerpentVersion();
		bool isLoaded();

	private:
		std::string name;
		std::string developer;
		std::string id;
		std::string version;
		std::string serpentVersion;
		bool loaded;
	};
	class SERPENTLUA_DLL Plugin final {
	public:
		static geode::Result<Plugin*, std::string> create(PluginMetadata* metadata, std::function<void(lua_State*)> onScriptLoaded);
		std::function<void(lua_State*)> getEntry();

		void setPlugin();
		int loadCount;
		PluginMetadata* metadata;
	private:
		std::function<void(lua_State*)> entry;
	};

	// only exporting this for plugins since its accessible through the serpentlua internal plugin
	struct SERPENTLUA_DLL ScriptMetadata final {
	public:
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
			static ScriptMetadata* create(std::map<std::string, std::string>& metadata);
			static geode::Result<ScriptMetadata*, std::string> createFromScript(const std::filesystem::path& scriptPath);
			void setPlugins();
			ScriptMetadata(){}
		#endif
		static geode::Result<ScriptMetadata*, std::string> getScriptByID(const std::string& id);
		static ScriptMetadata* getScriptByState(lua_State* L); // doesnt need to return a result because if the scriptmetadata doesnt exist then neither should the state (unless it was created manually)

		std::string getName();
		std::string getDeveloper();
		std::string getID();
		std::string getVersion();
		std::string getSerpentVersion();
		std::string getPath();
		std::vector<std::pair<std::string, std::string>> getPlugins();
		std::vector<std::string> getErrors();

	private:
		std::string name;
		std::string id;
		std::string version;
		std::string serpentVersion;
		std::string developer; // HOW the fuck did i forget this for this long
		std::string path;
		std::vector<std::pair<std::string, std::string>> plugins;
		std::string pluginIDstring;
	};
}