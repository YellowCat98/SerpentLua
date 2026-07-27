#include <internal/std/Format.hpp>
#include <internal/std/PluginEntry.hpp>
#include <fmt/args.h>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table ScriptBuiltin::Format::entry(sol::state_view state) {

	auto table = state.create_table();

	table["format"] = [](sol::this_state ts, const std::string& string, sol::variadic_args args) {
		sol::state_view lua(ts);
		auto tostring = lua["tostring"];

		fmt::dynamic_format_arg_store<fmt::format_context> store;
		for (const auto& arg : args) {
			switch (arg.get_type()) {
				case sol::type::string: {
					store.push_back(arg.as<std::string>());
					break;
				}
				case sol::type::number: {
					store.push_back(arg.as<double>());
					break;
				}
				case sol::type::boolean: {
					store.push_back(arg.as<bool>());
					break;
				}
				default: {
					store.push_back(tostring(arg).get<std::string>());
					break;
				}
			}
		}

		return fmt::vformat(string, store);
	};

	return table;
}