#include <internal/std/Log.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table ScriptBuiltin::Log::entry(sol::state_view state) {

	auto logging = state.create_table();

	std::function<void(sol::this_state, const std::string&, const std::string&)> logFn = [](sol::this_state ts, const std::string& msg, const std::string& type) {
		lua_State* L = ts;

		auto metadata = ScriptBuiltin::getMetadata(L);
		if (!metadata) {
			log::error("Metadata is null.");
			return;
		}
		auto name = metadata->name;

		if (type == "info") log::info("[SCRIPT] [{}]: {}", name, msg);
		else if (type == "debug") log::debug("[SCRIPT] [{}]: {}", name, msg);
		else if (type == "warn") log::warn("[SCRIPT] [{}]: {}", name, msg);
		else if (type == "error") log::error("[SCRIPT] [{}]: {}", name, msg);
		else if (type == "trace") log::trace("[SCRIPT] [{}]: {}", name, msg);
	};


	logging["info"] = [logFn](sol::this_state ts, const std::string& msg) -> void {
		logFn(ts, msg, "info");
	};

	logging["debug"] = [logFn](sol::this_state ts, const std::string& msg) -> void {
		logFn(ts, msg, "debug");
	};

	logging["trace"] = [logFn](sol::this_state ts, const std::string& msg) -> void {
		logFn(ts, msg, "trace");
	};

	logging["warn"] = [logFn](sol::this_state ts, const std::string& msg) -> void {
		logFn(ts, msg, "warn");
	};

	logging["error"] = [logFn](sol::this_state ts, const std::string& msg) -> void {
		logFn(ts, msg, "error");
	};

	return logging;
}