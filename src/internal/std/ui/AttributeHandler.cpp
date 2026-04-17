#include <internal/std/UI.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

template <typename T>
bool typeCheck(std::optional<sol::object> value, const std::string& name) {
	if (!value.has_value()) {
		log::warn("[UI PLUGIN] {} has no value.", name);
		return false;
	}
	if (!(*value).is<T>()) {
		log::warn("[UI PLUGIN] {} type mismatch.", name);
		return false;
	}
	return true;
}

static CCNode* getCCNode(sol::state_view state, ScriptBuiltin::ui::Node* node) {
	if (!node) return nullptr;
	return node->getNode(state);
}

void ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(sol::state_view state, Node* node) {
	if (!node) return;

	DEFINE_ADD_LAMBDA(add)

	add("setPositionX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setPositionX")) return sol::make_object(state, sol::nil);
		ccnode->setPositionX((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setPositionY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setPositionY")) return sol::make_object(state, sol::nil);
		ccnode->setPositionY((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setPosition", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setPosition")) return sol::make_object(state, sol::nil);
		auto t = (*value).as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setPosition table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setPosition(t["x"].get<float>(), t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScaleX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setScaleX")) return sol::make_object(state, sol::nil);
		ccnode->setScaleX((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScaleY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setScaleY")) return sol::make_object(state, sol::nil);
		ccnode->setScaleY((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScale", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setScale")) return sol::make_object(state, sol::nil);
		auto t = (*value).as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setScale table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setScaleX(t["x"].get<float>());
		ccnode->setScaleY(t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPointX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setAnchorPointX")) return sol::make_object(state, sol::nil);
		auto anchor = ccnode->getAnchorPoint();
		anchor.x = (*value).as<float>();
		ccnode->setAnchorPoint(anchor);
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPointY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setAnchorPointY")) return sol::make_object(state, sol::nil);
		auto anchor = ccnode->getAnchorPoint();
		anchor.y = (*value).as<float>();
		ccnode->setAnchorPoint(anchor);
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPoint", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setAnchorPoint")) return sol::make_object(state, sol::nil);
		auto t = (*value).as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setAnchorPoint table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setAnchorPoint({ t["x"].get<float>(), t["y"].get<float>() });
		return sol::make_object(state, sol::nil);
	});

	add("setContentWidth", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setContentWidth")) return sol::make_object(state, sol::nil);
		ccnode->setContentWidth((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setContentHeight", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setContentHeight")) return sol::make_object(state, sol::nil);
		ccnode->setContentHeight((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setContentSize", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setContentSize")) return sol::make_object(state, sol::nil);
		auto t = (*value).as<sol::table>();

		if (!t["width"].is<float>() || !t["height"].is<float>()) {
			log::warn("[UI PLUGIN] setContentSize table missing float width/height.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setContentSize({ t["width"].get<float>(), t["height"].get<float>() });
		return sol::make_object(state, sol::nil);
	});

	add("setRotationX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setRotationX")) return sol::make_object(state, sol::nil);
		ccnode->setRotationX((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setRotationY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setRotationY")) return sol::make_object(state, sol::nil);
		ccnode->setRotationY((*value).as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setRotation", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setRotation")) return sol::make_object(state, sol::nil);
		auto t = (*value).as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setRotation table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setRotationX(t["x"].get<float>());
		ccnode->setRotationY(t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setVisible", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<bool>(value, "setVisible")) return sol::make_object(state, sol::nil);
		ccnode->setVisible((*value).as<bool>());
		return sol::make_object(state, sol::nil);
	});

	add("setID", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<std::string>(value, "setID")) return sol::make_object(state, sol::nil);
		ccnode->setID((*value).as<std::string>());
		return sol::make_object(state, sol::nil);
	});



	add("getPositionX", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getPositionX());
	});

	add("getPositionY", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getPositionY());
	});

	add("getPosition", [state](CCNode* ccnode, std::optional<sol::object>) {
		auto pos = ccnode->getPosition();
		auto t = sol::state_view(state).create_table();
		t["x"] = pos.x;
		t["y"] = pos.y;
		return sol::make_object(state, t);
	});

	add("getScaleX", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getScaleX());
	});

	add("getScaleY", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getScaleY());
	});

	add("getScale", [state](CCNode* ccnode, std::optional<sol::object>) {
		auto t = sol::state_view(state).create_table();
		t["x"] = ccnode->getScaleX();
		t["y"] = ccnode->getScaleY();
		return sol::make_object(state, t);
	});

	add("getAnchorPointX", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getAnchorPoint().x);
	});

	add("getAnchorPointY", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getAnchorPoint().y);
	});

	add("getAnchorPoint", [state](CCNode* ccnode, std::optional<sol::object>) {
		auto anchor = ccnode->getAnchorPoint();
		auto t = sol::state_view(state).create_table();
		t["x"] = anchor.x;
		t["y"] = anchor.y;
		return sol::make_object(state, t);
	});

	add("getContentWidth", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getContentWidth());
	});

	add("getContentHeight", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getContentHeight());
	});

	add("getContentSize", [state](CCNode* ccnode, std::optional<sol::object>) {
		auto size = ccnode->getContentSize();
		auto t = sol::state_view(state).create_table();
		t["width"] = size.width;
		t["height"] = size.height;
		return sol::make_object(state, t);
	});

	add("getRotationX", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getRotationX());
	});

	add("getRotationY", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->getRotationY());
	});

	add("getRotation", [state](CCNode* ccnode, std::optional<sol::object>) {
		auto t = sol::state_view(state).create_table();
		t["x"] = ccnode->getRotationX();
		t["y"] = ccnode->getRotationY();
		return sol::make_object(state, t);
	});

	add("isVisible", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, ccnode->isVisible());
	});

	add("getID", [state](CCNode* ccnode, std::optional<sol::object>) {
		return sol::make_object(state, std::string(ccnode->getID()));
	});
}

void ScriptBuiltin::ui::AttributeHandler::populateAttributesSprite(sol::state_view state, Node *node) {

	DEFINE_ADD_LAMBDA(add)

	// ccnode inherits ccsprite anyway lolololololo!!!!!!
	ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, node);

	add("setColor", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<sol::table>(value, "setColor")) return sol::make_object(state, sol::nil);
		auto sprite = static_cast<CCSprite*>(ccnode);
		auto t = (*value).as<sol::table>();
		if (!t["r"].is<float>() || !t["g"].is<float>() || !t["b"].is<float>()) {
			log::warn("[UI PLUGIN] setRotation table missing float r/g/b.");
			return sol::make_object(state, sol::nil);
		}
		sprite->setColor({
			static_cast<GLubyte>(std::clamp(t["r"].get<float>(), 0.0f, 255.0f)),
			static_cast<GLubyte>(std::clamp(t["g"].get<float>(), 0.0f, 255.0f)),
			static_cast<GLubyte>(std::clamp(t["b"].get<float>(), 0.0f, 255.0f))
		});
		return sol::make_object(state, sol::nil);
	});

	add("setOpacity", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<float>(value, "setOpacity")) return sol::make_object(state, sol::nil);
		auto sprite = static_cast<CCSprite*>(ccnode);

		sprite->setOpacity(static_cast<GLubyte>(std::clamp((*value).as<float>(), 0.0f, 255.0f)));
		return sol::make_object(state, sol::nil);
	});

	add("setFlipX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<bool>(value, "setFlipX")) return sol::make_object(state, sol::nil);
		auto sprite = static_cast<CCSprite*>(ccnode);

		sprite->setFlipX((*value).as<bool>());
		return sol::make_object(state, sol::nil);
	});

	add("setFlipY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		if (!typeCheck<bool>(value, "setFlipY")) return sol::make_object(state, sol::nil);
		auto sprite = static_cast<CCSprite*>(ccnode);

		sprite->setFlipY((*value).as<bool>());
		return sol::make_object(state, sol::nil);
	});



	add("getColor", [state](CCNode* ccnode, std::optional<sol::object> value) {
		auto sprite = static_cast<CCSprite*>(ccnode);
		auto t = state.create_table_with(state,
			"r", static_cast<float>(sprite->getColor().r),
			"g", static_cast<float>(sprite->getColor().g),
			"b", static_cast<float>(sprite->getColor().b)
		);
		return sol::make_object(state, t);
	});

	add("getOpacity", [state](CCNode* ccnode, std::optional<sol::object> value) {
		auto sprite = static_cast<CCSprite*>(ccnode);
		return sol::make_object(state, static_cast<float>(sprite->getOpacity()));
	});

	add("isFlipX", [state](CCNode* ccnode, std::optional<sol::object> value) {
		auto sprite = static_cast<CCSprite*>(ccnode);
		return sol::make_object(state, sprite->isFlipX());
	});

	add("isFlipY", [state](CCNode* ccnode, std::optional<sol::object> value) {
		auto sprite = static_cast<CCSprite*>(ccnode);
		return sol::make_object(state, sprite->isFlipY());
	});
}

void ScriptBuiltin::ui::AttributeHandler::populateAttributesButton(sol::state_view state, Node *node) {
	return ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, node);
}

void ScriptBuiltin::ui::AttributeHandler::populateAttributesLabel(sol::state_view state, Node *node) {
	return ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, node);
}

void ScriptBuiltin::ui::AttributeHandler::populateAttributesMenu(sol::state_view state, Node *node) {
	return ScriptBuiltin::ui::AttributeHandler::populateAttributesNode(state, node);
}