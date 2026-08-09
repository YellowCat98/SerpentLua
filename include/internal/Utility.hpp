#pragma once

#include <SerpentLua.hpp>

namespace SerpentLua::internal {
	struct Utility {
		static bool versionInfoCompare(const geode::VersionInfo& first, const geode::VersionInfo& second);
		static geode::Result<std::string, std::string> handleVersion(const std::string& version); // parses version and adds a leading V

		static std::string sha256(std::vector<uint8_t> data);
		static std::pair<std::string, std::string> parseMetadataEntry(std::string metadata);
	};
}