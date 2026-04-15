#include <internal/std/UI.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table ScriptBuiltin::ui::entry(sol::state_view state) {
    auto table = state.create_table();

    table["helloUI"] = [](sol::this_state ts) {
        geode::log::info("hello worldicles");
    };

    return table;
}