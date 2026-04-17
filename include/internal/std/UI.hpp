#pragma once

#include <internal/SerpentLua.hpp>
#include <internal/std/Enums.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::ui {
	sol::table entry(sol::state_view state);

	struct Node;
	struct Attribute {
		std::function<sol::object(Node*, sol::object)> apply;

		Attribute(std::function<sol::object(Node*, sol::object)> apply) : apply(apply) {}
	};

	// functions that dont take this_state as their first argument dont get bound
	struct Node {
	public:
		static Node* create(sol::this_state ts, ScriptBuiltin::Enums::ui::NodeType type, sol::table options);
		static Node* getByID(sol::this_state ts, Node* from, const std::string& id, ScriptBuiltin::Enums::ui::NodeType type);
		static Node* createFromCCNode(sol::this_state ts, sol::object node, ScriptBuiltin::Enums::ui::NodeType type); // casts a CCNode to a Node so you can interact with it


		sol::object method(sol::this_state ts, const std::string& method, sol::object value);

		void addChild(sol::this_state ts, Node* node);
		void addToNode(sol::this_state ts, Node* node);

		cocos2d::CCNode* getNode(sol::state_view ts);
		ScriptBuiltin::Enums::ui::NodeType getType(sol::this_state ts);

		// not binding this but i need them in attribute handler
		std::unordered_map<std::string, Attribute> methods;
	private:
		cocos2d::CCNode* node; // casted to whatever type it is
		ScriptBuiltin::Enums::ui::NodeType type;
	};



	struct AttributeHandler {
		static void populateAttributesNode(sol::state_view ts, Node* node);
		static void populateAttributesSprite(sol::state_view ts, Node* node);
		static void populateAttributesButton(sol::state_view ts, Node* node);
		static void populateAttributesLabel(sol::state_view ts, Node* node);
		static void populateAttributesMenu(sol::state_view ts, Node* node);
	};
}