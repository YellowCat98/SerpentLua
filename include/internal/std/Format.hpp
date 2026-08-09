#pragma once

#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Format {
	sol::table entry(sol::state_view state);
}