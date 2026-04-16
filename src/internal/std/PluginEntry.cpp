#include <internal/std/PluginEntry.hpp>
#include <internal/std/Playground.hpp>
#include <internal/std/UI.hpp>
#include <internal/std/Enums.hpp>
#include <sol/sol.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

SerpentLua::ScriptMetadata* ScriptBuiltin::getMetadata(lua_State* L) {
	auto it = ScriptBuiltin::contexts.find(L);
	if (it == ScriptBuiltin::contexts.end()) return nullptr;

	return ScriptBuiltin::contexts.at(L).metadata;
}

void ScriptBuiltin::entry(lua_State* L) {
	sol::state_view state(L);

	auto& ctx = ScriptBuiltin::contexts[L];

	ctx.L = L;

	ctx.mainModule = state.create_table();

	ctx.metadata = RuntimeManager::get()->getScriptByState(L); // its guaranteed to be non-nullptr if the script was able to call ScriptBuiltin::entry

	auto _ScriptMetadata = ctx.mainModule.new_usertype<SerpentLua::ScriptMetadata>("ScriptMetadata", sol::no_constructor);

	_ScriptMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
		sol::state_view lua(ts); // ts so tuff!

		auto result = SerpentLua::ScriptMetadata::getScriptByID(id);

		if (result.isErr()) return sol::nil;

		return sol::make_object(lua, result.unwrap());
	};

	_ScriptMetadata["get"] = [](sol::this_state ts) -> sol::object {
		lua_State* L = ts;
		sol::state_view state(ts);
		return sol::make_object(ts, ScriptBuiltin::getMetadata(L));
	};

	_ScriptMetadata["name"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.name;
		}
	);
	_ScriptMetadata["id"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.id;
		}
	);
	_ScriptMetadata["version"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.version;
		}
	);
	_ScriptMetadata["serpentVersion"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.serpentVersion;
		}
	);
	_ScriptMetadata["nostd"] = sol::property(
		[](ScriptMetadata& self) -> bool& {
			return self.nostd;
		}
	);
	_ScriptMetadata["developer"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.developer;
		}
	);
	_ScriptMetadata["path"] = sol::property(
		[](ScriptMetadata& self) -> std::string& {
			return self.path;
		}
	);
	_ScriptMetadata["plugins"] = sol::property(
		[](ScriptMetadata& self) -> std::vector<std::string>& {
			return self.plugins;
		}
	);

	auto _PluginMetadata = ctx.mainModule.new_usertype<SerpentLua::PluginMetadata>("PluginMetadata", sol::no_constructor);

	_PluginMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
		sol::state_view lua(ts);

		auto res = RuntimeManager::get()->getPluginByID(id);

		if (res.isErr()) return sol::nil;

		return sol::make_object(lua, res.unwrap());
	};

	_PluginMetadata["name"] = sol::property(
		[](PluginMetadata& self) -> std::string& {
			return self.name;
		}
	);
	_PluginMetadata["id"] = sol::property(
		[](PluginMetadata& self) -> std::string& {
			return self.id;
		}
	);
	_PluginMetadata["version"] = sol::property(
		[](PluginMetadata& self) -> std::string& {
			return self.version;
		}
	);
	_PluginMetadata["serpentVersion"] = sol::property(
		[](PluginMetadata& self) -> std::string& {
			return self.serpentVersion;
		}
	);

	_PluginMetadata["developer"] = sol::property(
		[](PluginMetadata& self) -> std::string& {
			return self.developer;
		}
	);

	ctx.mainModule["log"] = logging(state);
	ctx.mainModule["playground"] = ScriptBuiltin::Playground::entry(state);
	ctx.mainModule["ui"] = ScriptBuiltin::ui::entry(state);
	ctx.mainModule["enums"] = ScriptBuiltin::Enums::entry(state);

	state["serpentlua_modules"]["serpentlua.std"] = ctx.mainModule;
}

sol::table ScriptBuiltin::logging(sol::state_view state) {

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

Result<> ScriptBuiltin::initPlugin() {
	if (ScriptBuiltin::plugin) return Err("Builtin plugin was already initialized.");
	auto metadata = SerpentLua::PluginMetadata::createFromMod(Mod::get());
	metadata->id = "serpentlua.std"; // i do not want to use create(std::map<std::string, std::string>) when this is very much simpler
	metadata->version = "v1.1.0";
	auto res = SerpentLua::Plugin::create(metadata, ScriptBuiltin::entry);
	if (res.isErr()) return Err("{}", res.err().value());

	ScriptBuiltin::plugin = res.unwrap();

	ScriptBuiltin::plugin->setPlugin();

	return Ok();
}