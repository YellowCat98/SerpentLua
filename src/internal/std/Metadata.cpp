#include <internal/std/Metadata.hpp>
#include <internal/std/PluginEntry.hpp>
#include <internal/RuntimeManager.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table ScriptBuiltin::Metadata::entry(sol::state_view state) {

	auto table = state.create_table();

	auto _ScriptMetadata = table.new_usertype<SerpentLua::ScriptMetadata>("ScriptMetadata", sol::no_constructor);

	_ScriptMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
		sol::state_view lua(ts); // ts so tuff!

		auto result = SerpentLua::ScriptMetadata::getScriptByID(id);

		if (result.isErr()) return sol::lua_nil;

		return sol::make_object(lua, result.unwrap());
	};

	_ScriptMetadata["get"] = [](sol::this_state ts) -> sol::object {
		lua_State* L = ts;
		sol::state_view state(ts);
		return sol::make_object(ts, ScriptBuiltin::getMetadata(L));
	};

	_ScriptMetadata["name"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getName();
		}
	);
	_ScriptMetadata["id"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getID();
		}
	);
	_ScriptMetadata["version"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getVersion();
		}
	);
	_ScriptMetadata["serpentVersion"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getSerpentVersion();
		}
	);
	_ScriptMetadata["developer"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getDeveloper();
		}
	);
	_ScriptMetadata["path"] = sol::property(
		[](ScriptMetadata& self) -> std::string {
			return self.getPath();
		}
	);
	_ScriptMetadata["plugins"] = sol::property(
		[](ScriptMetadata& self) {
			return sol::as_table(self.getPlugins());
		}
	);

	auto _PluginMetadata = table.new_usertype<SerpentLua::PluginMetadata>("PluginMetadata", sol::no_constructor);

	_PluginMetadata["getByID"] = [](sol::this_state ts, const std::string& id) -> sol::object {
		sol::state_view lua(ts);

		auto res = RuntimeManager::get()->getPluginByID(id);

		if (res.isErr()) return sol::lua_nil;

		return sol::make_object(lua, res.unwrap());
	};

	_PluginMetadata["name"] = sol::property(
		[](PluginMetadata& self) -> std::string {
			return self.getName();
		}
	);
	_PluginMetadata["id"] = sol::property(
		[](PluginMetadata& self) -> std::string {
			return self.getID();
		}
	);
	_PluginMetadata["version"] = sol::property(
		[](PluginMetadata& self) -> std::string {
			return self.getVersion();
		}
	);
	_PluginMetadata["serpentVersion"] = sol::property(
		[](PluginMetadata& self) -> std::string {
			return self.getSerpentVersion();
		}
	);

	_PluginMetadata["developer"] = sol::property(
		[](PluginMetadata& self) -> std::string {
			return self.getDeveloper();
		}
	);

	return table;
}