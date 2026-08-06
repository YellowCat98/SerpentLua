#pragma once
#include <string>
#include <vector>
#include <matjson.hpp>
#include <Geode/Geode.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <argon/argon.hpp>
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
		static PluginMetadata* create(std::map<std::string, std::string>& metadata);
		static PluginMetadata* createFromMod(geode::Mod* mod);
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
		static geode::Result<PluginMetadata*, std::string> createFromScript(const std::filesystem::path& path, bool enforceSameID = true);
		#endif
		std::string name;
		std::string developer;
		std::string id;
		std::string version;
		std::string serpentVersion;
		std::string path;
		bool native;
		bool loaded;
	};
	class SERPENTLUA_DLL Plugin final {
	public:
		static geode::Result<Plugin*, std::string> create(PluginMetadata* metadata, lua_State* state, std::function<bool()> entry, std::function<bool(lua_State*)> onScriptLoaded);
		std::function<bool()> getEntry();
		std::function<bool(lua_State*)> getOnScriptLoaded();
		lua_State* getLuaState();
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
		static geode::Result<Plugin*, std::string> createNative(const std::filesystem::path& path);
		void terminate();
		#endif

		void setPlugin();
		int loadCount;
		PluginMetadata* metadata;
	private:
		std::function<bool()> entry;
		std::function<bool(lua_State*)> onScriptLoaded;
		lua_State* state;
		std::string initScript;
		std::filesystem::path pathToUnzipped;
		bool native;
	};

	// only exporting this for plugins since its accessible through the serpentlua internal plugin
	struct SERPENTLUA_DLL ScriptMetadata final {
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
			static ScriptMetadata* create(std::map<std::string, std::string>& metadata);
			static geode::Result<ScriptMetadata*, std::string> createFromScript(const std::filesystem::path& scriptPath);
			void setPlugins();
			ScriptMetadata(){}
		#endif
		static geode::Result<ScriptMetadata*, std::string> getScriptByID(const std::string& id);
		static ScriptMetadata* getScriptByState(lua_State* L); // doesnt need to return a result because if the scriptmetadata doesnt exist then neither should the state (unless it was created manually)
		std::string name;
		std::string id;
		std::string version;
		std::string serpentVersion;
		std::string developer; // HOW the fuck did i forget this for this long
		bool nostd;
		std::string path;
		bool loaded;
		std::vector<std::pair<std::string, std::string>> plugins;
		std::string pluginIDstring;

		std::vector<std::string> errors;
	};
}