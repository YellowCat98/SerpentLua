#include <internal/std/PluginEntry.hpp>
#include <internal/std/Playground.hpp>
#include <internal/std/UI.hpp>
#include <internal/std/Enums.hpp>
#include <internal/std/Log.hpp>
#include <internal/std/Metadata.hpp>
#include <internal/std/Format.hpp>
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
	auto metadata = SerpentLua::PluginMetadata::createFromMod(Mod::get());
	metadata->id = "serpentlua.std"; // i do not want to use create(std::map<std::string, std::string>) when this is very much simpler
	metadata->version = "v1.4.0";
	auto res = SerpentLua::Plugin::create(metadata, ScriptBuiltin::entry);
	if (res.isErr()) return Err("{}", res.err().value());

	ScriptBuiltin::plugin = res.unwrap();

	ScriptBuiltin::plugin->setPlugin();

	return Ok();
}