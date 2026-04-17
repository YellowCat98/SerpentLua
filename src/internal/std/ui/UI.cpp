#include <internal/std/UI.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

CCNode* ScriptBuiltin::ui::Node::getNode(sol::state_view state) {
	return this->node;
}

ScriptBuiltin::Enums::ui::NodeType ScriptBuiltin::ui::Node::getType(sol::this_state state) {
	return this->type;
}

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::create(sol::this_state state, ScriptBuiltin::Enums::ui::NodeType type, sol::table options) {
	auto ret = new ScriptBuiltin::ui::Node();

	switch (type) {
		case ScriptBuiltin::Enums::ui::NodeType::Node: {
			ret->node = CCNode::create(); // options are #NotNeeded
			ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, ret);
			break;
		}

		case ScriptBuiltin::Enums::ui::NodeType::Sprite: {
			std::string spriteName = options["sprite"];
			bool frameName = options["frameName"];

			ret->node = frameName ? CCSprite::createWithSpriteFrameName(spriteName.c_str()) : CCSprite::create(spriteName.c_str());
			ScriptBuiltin::ui::AttributeHandler::populateAttributesSprite(state, ret);
			break;
		}

		case ScriptBuiltin::Enums::ui::NodeType::Button: {
			ScriptBuiltin::ui::Node* buttonImageAsNode = options["image"];
			sol::function callback = options["callback"];

			auto buttonImage = static_cast<CCNode*>(buttonImageAsNode->node);

			ret->node = CCMenuItemExt::createSpriteExtra(buttonImage, [callback](CCMenuItemSpriteExtra* sender) {
				return callback(sender);
			});
			ScriptBuiltin::ui::AttributeHandler::populateAttributesButton(state, ret);
			break;
		}

		case ScriptBuiltin::Enums::ui::NodeType::Label: {
			std::string text = options["text"];
			std::string font = options["font"];

			ret->node = CCLabelBMFont::create(text.c_str(), font.c_str());
			ScriptBuiltin::ui::AttributeHandler::populateAttributesLabel(state, ret);
			break;
		}

		case ScriptBuiltin::Enums::ui::NodeType::Menu: {
			ret->node = CCMenu::create();
			ScriptBuiltin::ui::AttributeHandler::populateAttributesMenu(state, ret);
			break;
		}
	}

	ret->type = type;

	return ret;
}

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::createFromCCNode(sol::this_state state, sol::object node, ScriptBuiltin::Enums::ui::NodeType type) {
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

ScriptBuiltin::ui::Node* ScriptBuiltin::ui::Node::getByID(sol::this_state state, ScriptBuiltin::ui::Node* from, const std::string& id, ScriptBuiltin::Enums::ui::NodeType type) {
	auto ccnode = from->node;

	return ScriptBuiltin::ui::Node::createFromCCNode(state, sol::make_object(state, ccnode->getChildByID(id)), type); // if the id doesnt exist itll just return nullptr lol
}

sol::object ScriptBuiltin::ui::Node::method(sol::this_state ts, const std::string& meth, sol::object value) {
	std::function<sol::object(sol::object)> REALApply = [this, meth](sol::object value) {
		auto attr = this->methods.at(meth);
		return attr.apply(this, value);
	};

	return sol::make_object(ts, REALApply);
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

void bindNodeWrappers(sol::state_view state, sol::table table) {
	table.set_function("createNode", [](sol::this_state ts) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Node, view.create_table());
	});

	table.set_function("createSprite", [](sol::this_state ts, const std::string& sprite, bool frameName) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Sprite, view.create_table_with(
			"sprite", sprite,
			"frameName", frameName
		));
	});

	table.set_function("createLabel", [](sol::this_state ts, const std::string& text, const std::string& font) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Label, view.create_table_with(
			"text", text,
			"font", font
		));
	});

	table.set_function("createButton", [](sol::this_state ts, ScriptBuiltin::ui::Node* node, sol::function callback) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Button, view.create_table_with(
			"image", node,
			"callback", callback
		));
	});

	table.set_function("createButtonWithSprite", [](sol::this_state ts, const std::string& sprite, bool frameName, sol::function callback) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Button, view.create_table_with(
			"image", ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Sprite, view.create_table_with(
				"sprite", sprite,
				"frameName", frameName
			)),
			"callback", callback
		));
	});

	table.set_function("createMenu", [](sol::this_state ts) {
		sol::state_view view(ts);
		return ScriptBuiltin::ui::Node::create(ts, ScriptBuiltin::Enums::ui::NodeType::Menu, view.create_table());
	});
}

sol::table ScriptBuiltin::ui::entry(sol::state_view state) {
	auto table = state.create_table();

	sol::table node_table = state.create_table();

	node_table.set_function("create", &ScriptBuiltin::ui::Node::create);
	node_table.set_function("getByID", sol::overload(
		[](sol::this_state ts, Node* from, const std::string& id) {
			return ScriptBuiltin::ui::Node::getByID(ts, from, id, ScriptBuiltin::Enums::ui::NodeType::Node);
		},
		[](sol::this_state ts, Node* from, const std::string& id, ScriptBuiltin::Enums::ui::NodeType type) {
			return ScriptBuiltin::ui::Node::getByID(ts, from, id, type);
		}
	));
	node_table.set_function("createFromCCNode", sol::overload(
		[](sol::this_state ts, sol::object node, ScriptBuiltin::Enums::ui::NodeType type) {
			return ScriptBuiltin::ui::Node::createFromCCNode(ts, node, type);
		},
		[](sol::this_state ts, sol::object node) {
			return ScriptBuiltin::ui::Node::createFromCCNode(ts, node, ScriptBuiltin::Enums::ui::NodeType::Node);
		}
	));

	bindNodeWrappers(state, table);
	table.set_function("getWinSize", [](sol::this_state ts) {
		sol::state_view state = ts;
		return state.create_table_with(
			"width", CCDirector::get()->getWinSize().width,
			"height", CCDirector::get()->getWinSize().height
		);
	});

	table["Node"] = node_table;

	state.new_usertype<ScriptBuiltin::ui::Node>("NodeInstance", sol::no_constructor,
		"method", &Node::method,
		"addChild", &Node::addChild,
		"addToNode", &Node::addToNode,
		"getType", &Node::getType
	);
	return table;
}