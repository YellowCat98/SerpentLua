#include <SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

PluginMetadata* PluginMetadata::create(std::map<std::string, std::string>& metadata) {
    auto ret = new PluginMetadata();
    ret->name = metadata["name"];
    ret->developer = metadata["developer"];
    ret->id = metadata["id"];
    ret->version = metadata["version"];
    ret->serpentVersion = metadata["serpent-version"];
    return ret;
}

