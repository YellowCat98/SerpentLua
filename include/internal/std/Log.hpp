#pragma once

#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Log {
	sol::table entry(sol::state_view state);
}