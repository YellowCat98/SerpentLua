#pragma once

#include <internal/SerpentLua.hpp>
#include <internal/std/Enums.hpp>
#include <sol/sol.hpp>

namespace SerpentLua::internal::ScriptBuiltin::ui {
	sol::table entry(sol::state_view state);

	struct Node;
	struct Attribute {
		sol::object value;
		std::function<void(Node*)> apply;

		Attribute(sol::object value, std::function<void(Node*)> apply) : value(value), apply(apply) {}
	};

	// functions that dont take this_state as their first argument dont get bound
	struct Node {
	public:
		static Node* create(sol::this_state ts, ScriptBuiltin::Enums::UI::NodeType type, sol::table options);
		static Node* getByID(sol::this_state ts, Node* from, const std::string& id, ScriptBuiltin::Enums::UI::NodeType type);
		static Node* createFromCCNode(sol::this_state ts, sol::object node, ScriptBuiltin::Enums::UI::NodeType type); // casts a CCNode to a Node so you can interact with it


		void setAttribute(sol::this_state ts, const std::string& attr, sol::object value);
		sol::object getAttribute(sol::this_state ts, const std::string& attr);

		sol::object callSpecial(sol::this_state ts, const std::string& special); // a special is just a function exclusive to that node! 
		void addChild(sol::this_state ts, Node* node);
		void addToNode(sol::this_state ts, Node* node);

		cocos2d::CCNode* getNode(sol::state_view ts);
		ScriptBuiltin::Enums::UI::NodeType getType(sol::this_state ts);

		// not binding these but i need them in attribute handler
		std::unordered_map<std::string, Attribute> attributes;
		std::unordered_map<std::string, sol::object> specials;
	private:
		cocos2d::CCNode* node; // casted to whatever type it is
		ScriptBuiltin::Enums::UI::NodeType type;

		void applyAttribute(sol::this_state ts, const std::string& attr, sol::object value);
	};



	struct AttributeHandler {
		static void populateAttributesNode(sol::state_view ts, Node* node);
		static void populateAttributesSprite(sol::state_view ts, Node* node);
		static void populateAttributesButton(sol::state_view ts, Node* node);
		static void populateAttributesLabel(sol::state_view ts, Node* node);
		static void populateAttributesMenu(sol::state_view ts, Node* node);
	};
}