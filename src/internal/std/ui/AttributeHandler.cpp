#include <internal/std/UI.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

void ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(sol::state_view state, Node* node) {
	if (!node || !node->getNode(state)) return;

	auto ccnode = node->getNode(state);

	node->attributes.insert_or_assign("position.x", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getPositionX()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setPositionX(node->attributes.at("position.x").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("position.y", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getPositionY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setPositionY(node->attributes.at("position.y").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("position", ScriptBuiltin::ui::Attribute{
		state.create_table_with("x", ccnode->getPositionX(), "y", ccnode->getPositionY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto t = node->attributes.at("position").value.as<sol::table>();
			ccnode->setPosition(t["x"], t["y"]);
		}
	});

	node->attributes.insert_or_assign("scale.x", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getScaleX()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setScaleX(node->attributes.at("scale.x").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("scale.y", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getScaleY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setScaleY(node->attributes.at("scale.y").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("scale", ScriptBuiltin::ui::Attribute{
		state.create_table_with("x", ccnode->getScaleX(), "y", ccnode->getScaleY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto t = node->attributes.at("scale").value.as<sol::table>();
			ccnode->setScaleX(t["x"]);
			ccnode->setScaleY(t["y"]);
		}
	});

	node->attributes.insert_or_assign("anchorPoint.x", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getAnchorPoint().x),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto anchor = ccnode->getAnchorPoint();
			anchor.x = node->attributes.at("anchorPoint.x").value.as<float>();
			ccnode->setAnchorPoint(anchor);
		}
	});

	node->attributes.insert_or_assign("anchorPoint.y", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getAnchorPoint().y),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto anchor = ccnode->getAnchorPoint();
			anchor.y = node->attributes.at("anchorPoint.y").value.as<float>();
			ccnode->setAnchorPoint(anchor);
		}
	});

	node->attributes.insert_or_assign("anchorPoint", ScriptBuiltin::ui::Attribute{
		state.create_table_with("x", ccnode->getAnchorPoint().x, "y", ccnode->getAnchorPoint().y),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto t = node->attributes.at("anchorPoint").value.as<sol::table>();
			ccnode->setAnchorPoint({ t["x"], t["y"] });
		}
	});

	node->attributes.insert_or_assign("contentSize.width", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getContentWidth()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto size = ccnode->getContentSize();
			size.width = node->attributes.at("contentSize.width").value.as<float>();
			ccnode->setContentSize(size);
		}
	});

	node->attributes.insert_or_assign("contentSize.height", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getContentHeight()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto size = ccnode->getContentSize();
			size.height = node->attributes.at("contentSize.height").value.as<float>();
			ccnode->setContentSize(size);
		}
	});

	node->attributes.insert_or_assign("contentSize", ScriptBuiltin::ui::Attribute{
		state.create_table_with("width", ccnode->getContentWidth(), "height", ccnode->getContentHeight()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto t = node->attributes.at("contentSize").value.as<sol::table>();
			ccnode->setContentSize({ t["width"], t["height"] });
		}
	});

	node->attributes.insert_or_assign("rotation.x", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getRotationX()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setRotationX(node->attributes.at("rotation.x").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("rotation.y", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getRotationY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setRotationY(node->attributes.at("rotation.y").value.as<float>());
		}
	});

	node->attributes.insert_or_assign("rotation", ScriptBuiltin::ui::Attribute{
		state.create_table_with("x", ccnode->getRotationX(), "y", ccnode->getRotationY()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			auto t = node->attributes.at("rotation").value.as<sol::table>();
			ccnode->setRotationX(t["x"]);
			ccnode->setRotationY(t["y"]);
		}
	});

	node->attributes.insert_or_assign("visible", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->isVisible()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setVisible(node->attributes.at("visible").value.as<bool>());
		}
	});

	node->attributes.insert_or_assign("id", ScriptBuiltin::ui::Attribute{
		sol::make_object(state, ccnode->getID()),
		[state](ScriptBuiltin::ui::Node* node) {
			if (!node) return;
			auto ccnode = node->getNode(state);
			if (!ccnode) return;

			ccnode->setID(node->attributes.at("id").value.as<std::string>());
		}
	});
}