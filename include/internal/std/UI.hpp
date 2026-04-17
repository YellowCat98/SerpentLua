#pragma once

#include <internal/SerpentLua.hpp>
#include <internal/std/Enums.hpp>
#include <sol/sol.hpp>

#define DEFINE_ADD_LAMBDA(name) \
std::function<void(const std::string&, std::function<sol::object(CCNode*, std::optional<sol::object>)>)> add = [&](const std::string& name, std::function<sol::object(CCNode*, std::optional<sol::object>)> fn) { \
	node->methods.insert_or_assign(name, ScriptBuiltin::ui::Attribute{ \
		[=](ScriptBuiltin::ui::Node* node, std::optional<sol::object> value) { \
			auto ccnode = getCCNode(state, node); \
			if (!ccnode) return sol::make_object(state, sol::nil); \
			return fn(ccnode, value); \
		} \
	}); \
}; \

namespace SerpentLua::internal::ScriptBuiltin::ui {
	sol::table entry(sol::state_view state);

	struct Node;
	struct Attribute {
		std::function<sol::object(Node*, std::optional<sol::object>)> apply;

		Attribute(std::function<sol::object(Node*, std::optional<sol::object>)> apply) : apply(apply) {}
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
		static void populateAttributesNode(sol::state_view state, Node* node);
		static void populateAttributesSprite(sol::state_view state, Node* node);
		static void populateAttributesButton(sol::state_view state, Node* node);
		static void populateAttributesLabel(sol::state_view state, Node* node);
		static void populateAttributesMenu(sol::state_view state, Node* node);
	};
}