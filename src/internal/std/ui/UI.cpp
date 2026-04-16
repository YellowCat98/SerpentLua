#include <internal/std/UI.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

CCNode* ScriptBuiltin::ui::Node::getNode(sol::state_view state) {
	return this->node;
}

ScriptBuiltin::Enums::UI::NodeType ScriptBuiltin::ui::Node::getType(sol::this_state state) {
	return this->type;
}

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::create(sol::this_state state, ScriptBuiltin::Enums::UI::NodeType type, sol::table options) {
	auto ret = new ScriptBuiltin::ui::Node();

	switch (type) {
		case ScriptBuiltin::Enums::UI::NodeType::Node: {
			ret->node = CCNode::create(); // options are #NotNeeded
			break;
		}

		case ScriptBuiltin::Enums::UI::NodeType::Sprite: {
			std::string spriteName = options["sprite"];
			bool frameName = options["frameName"];

			ret->node = frameName ? CCSprite::createWithSpriteFrameName(spriteName.c_str()) : CCSprite::create(spriteName.c_str());
			break;
		}

		case ScriptBuiltin::Enums::UI::NodeType::Button: {
			ScriptBuiltin::ui::Node* buttonImageAsNode = options["image"];
			sol::function callback = options["callback"];

			auto buttonImage = static_cast<CCNode*>(buttonImageAsNode->node);

			ret->node = CCMenuItemExt::createSpriteExtra(buttonImage, [callback](CCMenuItemSpriteExtra* sender) {
				return callback(sender);
			});
			break;
		}

		case ScriptBuiltin::Enums::UI::NodeType::Label: {
			std::string text = options["text"];
			std::string font = options["font"];

			ret->node = CCLabelBMFont::create(text.c_str(), font.c_str());
			break;
		}

		case ScriptBuiltin::Enums::UI::NodeType::Menu: {
			ret->node = CCMenu::create();
			break;
		}
	}

	ret->type = type;

	ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, ret);

	return ret;
}

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::createFromCCNode(sol::this_state state, sol::object node, ScriptBuiltin::Enums::UI::NodeType type) {
	auto ret = new ScriptBuiltin::ui::Node();

	ret->type = type;

    lua_State* L = state;
    node.push(L);
    CCNode* rawNode = *static_cast<CCNode**>(lua_touserdata(L, -1));
    lua_pop(L, 1);

	ret->node = rawNode;

	ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, ret);

	return ret;
}

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::getByID(sol::this_state state, ScriptBuiltin::ui::Node* from, const std::string& id, ScriptBuiltin::Enums::UI::NodeType type) {
	auto ccnode = from->node;

	return ScriptBuiltin::ui::Node::createFromCCNode(state, sol::make_object(state, ccnode->getChildByID(id)), type); // if the id doesnt exist itll just return nullptr lol
}

void ScriptBuiltin::ui::Node::setAttribute(sol::this_state state, const std::string& attr, sol::object value) {
	if (auto it = this->attributes.find(attr); it != this->attributes.end()) {
		it->second.value = value;
		this->applyAttribute(state, attr, value);
	}
}

sol::object ScriptBuiltin::ui::Node::getAttribute(sol::this_state state, const std::string& attr) {
	if (!this->attributes.contains(attr)) {
		return sol::nil;
	}

	return this->attributes.at(attr).value;
}

void ScriptBuiltin::ui::Node::applyAttribute(sol::this_state state, const std::string& attr, sol::object value) {
	if (!this->node) return;

	if (!this->attributes.contains(attr)) return;

	this->attributes.at(attr).apply(this);
}


void ScriptBuiltin::ui::Node::addChild(sol::this_state state, Node* node) {
	if (!this->node) return;
	if (!node || !node->getNode(state)) return;

	this->node->addChild(node->getNode(state));
}

void ScriptBuiltin::ui::Node::addToNode(sol::this_state state, Node* node) {
	if (!this->node) return;
	if (!node || !node->getNode(state)) return;

	node->getNode(state)->addChild(this->node);
}

sol::table ScriptBuiltin::ui::entry(sol::state_view state) {
	auto table = state.create_table();

	sol::table node_table = state.create_table();

	node_table.set_function("create", &ScriptBuiltin::ui::Node::create);
	node_table.set_function("getByID", &ScriptBuiltin::ui::Node::getByID);
	node_table.set_function("createFromCCNode", &ScriptBuiltin::ui::Node::createFromCCNode);

	table["Node"] = node_table;

	state.new_usertype<ScriptBuiltin::ui::Node>("NodeInstance", sol::no_constructor,
		"setAttribute", &Node::setAttribute,
		"getAttribute", &Node::getAttribute,

		"addChild", &Node::addChild,
		"addToNode", &Node::addToNode,
		"getType", &Node::getType
	);
	return table;
}