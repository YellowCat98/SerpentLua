#include <internal/std/Enums.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table createUITable(sol::state_view state) {
	auto table = state.create_table();

	// NodeType
	table.new_enum("NodeType",
		"Node", ScriptBuiltin::Enums::ui::NodeType::Node,
		"Sprite", ScriptBuiltin::Enums::ui::NodeType::Sprite,
		"Button", ScriptBuiltin::Enums::ui::NodeType::Button,
		"Label", ScriptBuiltin::Enums::ui::NodeType::Label,
		"Menu", ScriptBuiltin::Enums::ui::NodeType::Menu
	);

	return table;
}

sol::table ScriptBuiltin::Enums::entry(sol::state_view state) {
	auto table = state.create_table();

	table["ui"] = createUITable(state);

	return table;
}