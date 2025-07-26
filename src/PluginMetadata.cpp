#include <SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua;

PluginMetadata PluginMetadata::create(std::map<std::string, std::string>& metadata) {
    auto ret = PluginMetadata();
    ret.name = metadata["name"];
    ret.id = metadata["id"];
    ret.version = metadata["version"];
    ret.serpentVersion = metadata["serpent-version"];
    return ret;
}

