#include <internal/SerpentLua.hpp>
#include <libs/picosha2/picosha2.h>

using namespace SerpentLua::internal;
using namespace geode::prelude;

bool utility::versionInfoCompare(const geode::VersionInfo& first, const geode::VersionInfo& second) {
    if (first.getMajor() != second.getMajor()) return false;
    if (second.getMinor() < first.getMinor()) return false;

    return true;
}

geode::Result<std::string, std::string> utility::handleVersion(const std::string& version) {
    auto res = VersionInfo::parse(version);
    if (res.isErr()) return Err(*(res.err()));

    return Ok(res.unwrap().toVString());
}

std::string utility::sha256(std::vector<uint8_t> data) {
	std::vector<uint8_t> hashVec(picosha2::k_digest_size);
	picosha2::hash256(data.begin(), data.end(), hashVec);
	return picosha2::bytes_to_hex_string(hashVec.begin(), hashVec.end());
}

std::pair<std::string, std::string> utility::parseMetadataEntry(std::string line) {
	if (line.rfind("--@", 0) != 0) return std::pair<std::string, std::string>({});
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
	return {key, value};
}