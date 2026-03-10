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
        std::vector<Plugin*> pendingPlugins; // this only exists so a failed script doesnt keep a plugin in use
        bool executed;
        bool pluginsInitiated;
        void terminate(); // currently only used to terminate scripts that failed execution.
        void commitLoadedPlugins();
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

    class OpenScriptsSettingV3 : public geode::SettingV3 {
    public:
        static geode::Result<std::shared_ptr<geode::SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json);
        bool load(matjson::Value const& json) override;
        bool save(matjson::Value& json) const override;
        bool isDefaultValue() const override;
        void reset() override;
        geode::SettingNodeV3* createNode(float width) override;
    };

    class OpenScriptsSettingNodeV3 : public geode::SettingNodeV3 {
    protected:
        bool init(std::shared_ptr<OpenScriptsSettingV3> setting, float width);
        void updateState(cocos2d::CCNode* invoker) override;
        void onCommit() override;
        void onResetToDefault() override;

    public:
        static OpenScriptsSettingNodeV3* create(std::shared_ptr<OpenScriptsSettingV3> setting, float width);
        bool hasUncommittedChanges() const override;
        bool hasNonDefaultValue() const override;
    };
};