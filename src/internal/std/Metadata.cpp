#include <internal/std/Metadata.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table ScriptBuiltin::Metadata::entry(sol::state_view state) {

	auto table = state.create_table();

	auto _ScriptMetadata = table.new_usertype<SerpentLua::ScriptMetadata>("ScriptMetadata", sol::no_constructor);

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
		[](ScriptMetadata& self) {
			return sol::as_table(self.plugins);
		}
	);

	auto _PluginMetadata = table.new_usertype<SerpentLua::PluginMetadata>("PluginMetadata", sol::no_constructor);

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

	return table;
}