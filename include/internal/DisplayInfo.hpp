#pragma once

#include <SerpentLua.hpp>

namespace SerpentLua::internal {
	// universal metadata for both scripts, plugins, and serverside plugins! this is meant for ui
	struct DisplayInfo {
		static DisplayInfo create(matjson::Value metadata);
		static DisplayInfo createFromScript(void* script, bool isScript);

		std::string name;
		std::string developer;
		std::string id;
		std::string version;
		std::string serpentVersion;
		std::string description;

		std::string downloadLink;
		std::string scriptExample;
		std::string downloadHash; // might need this idk
		std::string scriptDownloadHash;
		std::string filename;
		std::string scriptFilename;

		std::string source;
		std::string status;

		int downloadCount = 0;
		int accountId = 0;

		int64_t releaseDate = 0;
		int64_t lastUpdateDate = 0;

		bool featured;

		std::string path;
		bool native;
		bool loaded;
		bool script;

		std::variant<ScriptMetadata*, PluginMetadata*> internal; // represents the ScriptMetadata or PluginMetadata, nullptr if index.
	};
}