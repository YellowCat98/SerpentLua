#pragma once

#include <SerpentLua.hpp>

namespace SerpentLua::internal {
	// an amazing struct that exists for stuff that happens when the mod is loading!
	struct StartupOperations {
		static void installPending(bool scripts);
		static void loadScripts();
		static void loadNativePlugins();
		static void unfortunatelyDeleteTheUnfortunates();
	};
}