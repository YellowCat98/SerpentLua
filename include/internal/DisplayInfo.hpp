#pragma once

#include <SerpentLua.hpp>

namespace SerpentLua::internal {
	// this is mainly just universal metadata
	struct DisplayInfo {
		static DisplayInfo createFromScript(void* script, bool isScript);

		std::string name;
		std::string developer;
		std::string id;
		std::string version;
		std::string serpentVersion;
		std::string description;

		std::string path;
		bool loaded;
		bool script;

		std::variant<ScriptMetadata*, PluginMetadata*> internal; // represents the ScriptMetadata or PluginMetadata
	};
}