#pragma once
#include <string>
#include <vector>
#include <matjson.hpp>
#include <Geode/Geode.hpp>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

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
		static geode::Result<Plugin*, std::string> create(PluginMetadata* metadata, std::function<void(lua_State*)> entry);
		std::function<void(lua_State*)> getEntry();
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
		static geode::Result<Plugin*, std::string> createNative(const std::filesystem::path& path); // meant for plugins that are built using dlls explicitly.
		void terminate();
		#endif

		//static geode::Result<PluginMetadata*, std::string> getPluginByID(const std::string& id);

		void setPlugin(); // will set the Pulgin.
		int loadCount; // helps determine whether to terminate the plugin if no script uses it, helps save memory.
		// instead of a simple `loadedSomewhere` bool, we count how many times a script loaded it!
		PluginMetadata* metadata;
	private:
		std::function<void(lua_State*)> entry;
		bool native;


		std::optional<HMODULE> hDll;
		// meant for native plugins.
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
		// __metadata is used for both plugins and scripts
		struct __metadata {
			const char* name;
			const char* developer;
			const char* id;
			const char* version;
			const char* serpentVersion;
			const char** plugins;
			int pluginsSize;
		};

		struct SerpentLuaAPI {
			void (*log)(__metadata, const char*, const char*);
			__metadata (*get_script)(lua_State*);
			__metadata metadata;
			HMODULE handle;
		};

		struct SerpentLuaAPIImpl {
			static void log(__metadata, const char* msg, const char* type);
			static __metadata get_script(lua_State* L);
		};
		#endif
	};

	// only exporting this for plugins since its accessible through the serpentlua internal plugin
	struct SERPENTLUA_DLL ScriptMetadata final {
		#ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
			static ScriptMetadata* create(std::map<std::string, std::string>& metadata);
			static geode::Result<ScriptMetadata*, std::string> createFromScript(const std::filesystem::path& scriptPath);
			static std::pair<std::string, std::string> createPair(std::string& lines);
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
		std::vector<std::string> plugins;
		std::string pluginIDstring;

		std::vector<std::string> errors;
	};

	namespace utility {
		bool versionInfoCompare(const geode::VersionInfo& first, const geode::VersionInfo& second);
		geode::Result<std::string, std::string> handleVersion(const std::string& version); // parses version and adds a leading V
	};

}