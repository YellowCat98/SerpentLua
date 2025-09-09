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


    struct SERPENTLUA_DLL PluginMetadata final {
        static PluginMetadata* create(std::map<std::string, std::string>& metadata);
        std::string name;
        std::string developer;
        std::string id;
        std::string version;
        std::string serpentVersion;
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
        struct __metadata {
            const char* name;
            const char* developer;
            const char* id;
            const char* version;
            const char* serpentVersion;
        };
        #endif
    };

    // only exporting this for plugins since its accessible through the serpentlua internal plugin
    struct SERPENTLUA_DLL ScriptMetadata final {
        #ifdef YELLOWCAT98_SERPENTLUA_EXPORTING
            static ScriptMetadata* create(std::map<std::string, std::string>& metadata);
            static geode::Result<ScriptMetadata*, std::string> createFromScript(const std::filesystem::path& scriptPath);
            static geode::Result<ScriptMetadata*, std::string> getScript(const std::string& id);
            static std::pair<std::string, std::string> createPair(std::string& lines);
            void setPlugins();
            ScriptMetadata(){}
        #endif
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

}