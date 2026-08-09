#pragma once

#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Metadata {
	sol::table entry(sol::state_view state);
}