#include <internal/SerpentLua.hpp>
#include <Geode/Geode.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void SerpentLua::ScriptMetadata::setPlugins() {
    if (this->pluginIDstring.empty()) return;
    std::vector<std::string> pluginIDs;
    size_t start = 0;
    size_t end = this->pluginIDstring.find(' ');

    while (end != std::string::npos) {
        pluginIDs.push_back(this->pluginIDstring.substr(start, end - start));
        start = end + 1;
        end = this->pluginIDstring.find(' ', start);
    }
    pluginIDs.push_back(this->pluginIDstring.substr(start));

    this->plugins = pluginIDs; // would be better to use metadata->plugins as pluginIDs from the start but i already wrote everything i do not want to replace the stuff
}

geode::Result<SerpentLua::ScriptMetadata*, std::string> SerpentLua::ScriptMetadata::getScript(const std::string& id) {
    return SerpentLua::internal::RuntimeManager::get()->getScriptByID(id);
}

geode::Result<SerpentLua::ScriptMetadata*, std::string> SerpentLua::ScriptMetadata::createFromScript(const std::filesystem::path& scriptPath) {
    if (!std::filesystem::exists(scriptPath)) return Err("Script {} doesn't exist.", scriptPath.filename());
    log::info("Script `{}` creation: Initialized.", scriptPath.filename());
    std::ifstream file(scriptPath);
    if (!file.is_open()) return Err("Script `{}` creation: an error occured opening file.", scriptPath.filename());

    std::vector<std::string> lines;
    std::string line;
    size_t count = 0;
    size_t max = 7;

    while (std::getline(file, line) && count < max) {
        lines.push_back(line);
        ++count;
    }

    // now to PARSING
    std::map<std::string, std::string> metadata; // turn the lines into a more readable format
    for (auto& line : lines) {
        if (std::all_of(line.begin(), line.end(), [](unsigned char c) {
            return std::isspace(c);
        })) continue;
        if (line.rfind("--@", 0) != 0) return Err("Script `{}` metadata: The first {} lines in a script must be metadata.", scriptPath.filename(), max+1);
        line.erase(0,3);
        
        std::string key, value; // now we separate the data!
        
        auto pos = line.find(' '); // keys and values are separated with SPACES

        if (pos != std::string::npos) {
            key = line.substr(0, pos);
            value = line.substr(pos + 1);
        } else {
            key = line;
            value = "";
        }

        if (metadata.contains(key)) return Err("Script `{}` metadata: Metadata already contains key {}", scriptPath.filename(), key);

        metadata.insert({key,value});
    }
    log::debug("Script `{}` metadata: Retrieved metadata, checking metadata validity...", scriptPath.filename());


    // checking for metadata validity
    std::vector<std::string> requiredKeys = {"name", "id", "version", "serpent-version", "developer"};

    for (auto& req : requiredKeys) {
        if (!metadata.contains(req)) return Err("Script `{}` metadata: Metadata is missing `{}` key.", scriptPath.filename(), req);
    }

    requiredKeys.push_back("nostd"); // nostd is not required, im only adding nostd to it because requiredKeys is now repurposed to check for unknown keys!
    requiredKeys.push_back("plugins"); // same for this one
    for (const auto& [key, value] : metadata) { // this also disallows things like `--@  developer hello`
        auto it = std::find(requiredKeys.begin(), requiredKeys.end(), key);
        if (it == requiredKeys.end()) return Err("Script `{}` metadata: Unknown Metadata key: {}", scriptPath.filename(), key);
    }
    if (scriptPath.extension().string() == ".luac") metadata.insert({"isLuac","true"});
    // HOW the fuck did i forget this holy fucking SHIT.
    metadata.insert({"path", scriptPath.string()});

    log::debug("Script `{}` metadata: Metadata gathered.", scriptPath.filename());

    return Ok(ScriptMetadata::create(metadata));
}

SerpentLua::ScriptMetadata* SerpentLua::ScriptMetadata::create(std::map<std::string, std::string>& metadata) {
    auto ret = new SerpentLua::ScriptMetadata();

    ret->name = metadata["name"];
    ret->id = metadata["id"];
    ret->version = metadata["version"];
    ret->serpentVersion = metadata["serpent-version"];
    ret->nostd = metadata.contains("nostd");
    ret->path = metadata["path"];
    if (metadata.contains("plugins")) ret->pluginIDstring = metadata["plugins"];
    else ret->pluginIDstring = "";
    ret->developer = metadata["developer"];
    ret->isLuac = metadata.contains("isLuac");
    ret->setPlugins();

    return ret;
}