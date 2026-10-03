#include "Geode/utils/file.hpp"
#include <internal/std/PluginEntry.hpp>
#include <internal/std/Playground.hpp>
#include <internal/std/UI.hpp>
#include <internal/std/Enums.hpp>
#include <internal/std/Log.hpp>
#include <internal/std/Metadata.hpp>
#include <internal/std/Format.hpp>
#include <sol/sol.hpp>
#include <internal/RuntimeManager.hpp>

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

	auto md = ScriptBuiltin::Metadata::entry(state);
	ctx.mainModule["ScriptMetadata"] = md["ScriptMetadata"];
	ctx.mainModule["PluginMetadata"] = md["PluginMetadata"];
	ctx.mainModule["log"] = ScriptBuiltin::Log::entry(state);
	ctx.mainModule["playground"] = ScriptBuiltin::Playground::entry(state);
	ctx.mainModule["ui"] = ScriptBuiltin::ui::entry(state);
	ctx.mainModule["fmt"] = ScriptBuiltin::Format::entry(state);
	ctx.mainModule["enums"] = ScriptBuiltin::Enums::entry(state);

	state["serpentlua_modules"]["serpentlua.std"] = ctx.mainModule;
}

Result<> ScriptBuiltin::initPlugin() {
	if (ScriptBuiltin::plugin) return Err("Builtin plugin was already initialized.");

	auto mdPath = Mod::get()->getResourcesDir() / "stdpluginmd.json"; // horrible name i know

	auto mdRes = utils::file::readJson(mdPath);
	if (mdRes.isErr()) return Err("Unable to read Builtin plugin metadata: {}", mdRes.unwrapErr());

	auto md = mdRes.unwrap();

	// since stdpluginmd.json can be written to, we have to check each item separately!

	GEODE_UNWRAP_INTO(auto name, md["name"].asString().mapErr([](auto const& err) {
		return fmt::format("Unable to get builtin plugin metadata `name` property: {}", err);
	}));

	GEODE_UNWRAP_INTO(auto developer, md["developer"].asString().mapErr([](auto const& err) {
		return fmt::format("Unable to get builtin plugin metadata `developer` property: {}", err);
	}));

	GEODE_UNWRAP_INTO(auto id, md["id"].asString().mapErr([](auto const& err) {
		return fmt::format("Unable to get builtin plugin metadata `id` property: {}", err);
	}));

	GEODE_UNWRAP_INTO(auto version, md["version"].asString().mapErr([](auto const& err) {
		return fmt::format("Unable to get builtin plugin metadata `version` property: {}", err);
	}));

	GEODE_UNWRAP_INTO(auto serpentVersion, md["serpent-version"].asString().mapErr([](auto const& err) {
		return fmt::format("Unable to get builtin plugin metadata `serpent-version` property: {}", err);
	}));

	auto metadata = SerpentLua::PluginMetadata::create({
		{"name", name},
		{"developer", developer},
		{"id", id},
		{"version", version},
		{"serpent-version", serpentVersion}
	});
	auto res = SerpentLua::Plugin::create(metadata, &ScriptBuiltin::entry);
	if (res.isErr()) return Err("{}", res.err().value());

	ScriptBuiltin::plugin = res.unwrap();

	ScriptBuiltin::plugin->setPlugin();

	return Ok();
}