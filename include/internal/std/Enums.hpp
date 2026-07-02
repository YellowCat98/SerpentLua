#pragma once

#include <internal/SerpentLua.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::Enums {
	sol::table entry(sol::state_view ts);

	struct ui {
		enum class NodeType {
			Node, // CCNode
			Sprite, // CCSprite
			Button, // CCMenuItemSpriteExtra (using CCMenuItemExt::createSpriteExtra method)
			Label, // CCLabelBMFont
			Menu, // CCMenu
			Alert, // FLAlertLayer
			Notification // Notification
		};

		// pretend NotificationIcon is here too because it gets bound too
	};
};