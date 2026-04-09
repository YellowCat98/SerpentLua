#include <SerpentLua.hpp>

using namespace SerpentLua;
using namespace geode::prelude;

bool utility::versionInfoCompare(const geode::VersionInfo& first, const geode::VersionInfo& second) {
    return first.getMajor() == second.getMajor();
}

geode::Result<std::string, std::string> utility::handleVersion(const std::string& version) {
    auto res = VersionInfo::parse(version);
    if (res.isErr()) return Err(*(res.err()));

    return Ok(res.unwrap().toVString());
}