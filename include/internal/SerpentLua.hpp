// internal header, meant for serpentlua's plugin.
#pragma once
#define SOL_ALL_SAFETIES_ON 1
#define SOL_USE_STD_OPTIONAL 1
#include <SerpentLua.hpp>

namespace SerpentLua::internal {
    class script {
    public:
        static geode::Result<script*, std::string> create(ScriptMetadata* metadata);
        static geode::Result<script*, std::string> getLoadedScript(const std::string& id);
        ScriptMetadata* getMetadata();
        lua_State* getLuaState();
        lua_State* createState();
        geode::Result<> execute(); // Executes THE SCRIPT.
        geode::Result<> loadPlugins();
    private:
        ScriptMetadata* metadata;
        lua_State* state; // each script requires its own lua state for guaranteed isolation. (and also so you can get a script by just a function call!)
        std::vector<Plugin*> plugins;
        bool executed;
        bool pluginsInitiated;
        void terminate(); // currently only used to terminate scripts that failed execution.
    };

    class RuntimeManager {
    public:
        static RuntimeManager* get();
        geode::Result<script*, std::string> getLoadedScriptByID(const std::string& id); // script::getLoadedScriptByID;
        geode::Result<ScriptMetadata*, std::string> getScriptByID(const std::string& id); // ScriptMetadata::getScriptByID();

        geode::Result<Plugin*, std::string> getLoadedPluginByID(const std::string& id);
        //geode::Result<PluginMetadata*, std::string> getPluginByID(const std::string& id);

        void setLoadedScript(script* script);
        void setScript(ScriptMetadata* script);

        void setPlugin(const std::string& id, Plugin* plugin);

        std::map<std::string, script*> getAllLoadedScripts();
        std::map<std::string, ScriptMetadata*> getAllScripts();

        std::map<std::string, SerpentLua::Plugin*> getAllLoadedPlugins();

        geode::Result<> removeLoadedScript(const std::string& id);

        geode::Result<> removeLoadedPlugin(const std::string& id);
    private:
        // using std::map so i can retrieve a script directly through id
        std::map<std::string, ScriptMetadata*> scripts;
        std::map<std::string, script*> loadedScripts;

        //std::map<std::string, SerpentLua::PluginMetadata*> plugins;
        std::map<std::string, SerpentLua::Plugin*> loadedPlugins;
    };

    namespace ScriptBuiltin { // builtin plugin, can be included within a script by doing `--@plugins serpentlua_builtin`
        void entry(lua_State* L);

        inline SerpentLua::Plugin* plugin = nullptr;
        geode::Result<> initPlugin();
    };
};