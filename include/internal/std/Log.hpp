#pragma once

#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Log {
	sol::table entry(sol::state_view state);
}