#include <internal/std/Enums.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

sol::table createUITable(sol::state_view state) {
	auto table = state.create_table();

	// NodeType
	table.new_enum("NodeType",
		"Node", ScriptBuiltin::Enums::UI::NodeType::Node,
		"Sprite", ScriptBuiltin::Enums::UI::NodeType::Sprite,
		"Button", ScriptBuiltin::Enums::UI::NodeType::Button,
		"Label", ScriptBuiltin::Enums::UI::NodeType::Label,
		"Menu", ScriptBuiltin::Enums::UI::NodeType::Menu
	);

	return table;
}

sol::table ScriptBuiltin::Enums::entry(sol::state_view state) {
	auto table = state.create_table();

	table["ui"] = createUITable(state);

	return table;
}