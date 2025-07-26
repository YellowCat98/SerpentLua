// internal header, meant for serpentlua's plugin.
#pragma once
#include <SerpentLua.hpp>

namespace SerpentLua::internal {
    struct ScriptMetadata {
        static ScriptMetadata create(std::map<std::string, std::string>& metadata);
        static geode::Result<ScriptMetadata, std::string> createFromScript(const std::filesystem::path& scriptPath);
        static geode::Result<ScriptMetadata, std::string> getScript(const std::string& id);
        std::string name;
        std::string id;
        std::string version;
        std::string serpentVersion;
        bool nostd;
        std::filesystem::path path;
        std::string status;
    };
    class script {
    public:
        static geode::Result<script*, std::string> create(const ScriptMetadata& metadata);
        static geode::Result<script*, std::string> getLoadedScript(const std::string& id);
        ScriptMetadata getMetadata();
        lua_State* getLuaState();
        static lua_State* createState(bool nostd);
        geode::Result<> execute(); // Executes THE SCRIPT.
    private:
        ScriptMetadata metadata;
        lua_State* state; // each script requires its own lua state for guaranteed isolation.
        bool executed;
        bool pluginsInitiated;
        void terminate(); // currently only used to terminate scripts that failed execution.
    };

    class RuntimeManager {
    public:
        static RuntimeManager* get();
        geode::Result<script*, std::string> getLoadedScriptByID(const std::string& id); // script::getLoadedScriptByID;
        geode::Result<ScriptMetadata, std::string> getScriptByID(const std::string& id); // ScriptMetadata::getScriptByID();

        void setLoadedScript(script* script);
        void setScript(const ScriptMetadata& script);
    private:
        // using std::map so i can retrieve a script directly through id
        std::map<std::string, ScriptMetadata> scripts;
        std::map<std::string, script*> loadedScripts;
    };

    namespace ScriptBuiltin { // builtin plugin, can be included within a script by doing `--@plugins serpentlua_builtin`
        void entry(lua_State* L);

        inline SerpentLua::Plugin* plugin = nullptr;
        geode::Result<> initPlugin();
    };
};