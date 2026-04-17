#include <internal/std/ui.hpp>
#include <internal/std/PluginEntry.hpp>

using namespace SerpentLua::internal;
using namespace geode::prelude;

template <typename T>
bool typeCheck(sol::object value, const char* name) {
	if (!value.is<T>()) {
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

	std::function<void(const std::string&, std::function<sol::object(CCNode*, sol::object)>)> add = [&](const std::string& name, std::function<sol::object(CCNode*, sol::object)> fn) {
		node->methods.insert_or_assign(name, ScriptBuiltin::ui::Attribute{
			[=](ScriptBuiltin::ui::Node* node, sol::object value) {
				auto ccnode = getCCNode(state, node);
				if (!ccnode) return sol::make_object(state, sol::nil);
				return fn(ccnode, value);
			}
		});
	};

	add("setPositionX", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setPositionX")) return sol::make_object(state, sol::nil);
		ccnode->setPositionX(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setPositionY", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setPositionY")) return sol::make_object(state, sol::nil);
		ccnode->setPositionY(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setPosition", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<sol::table>(value, "setPosition")) return sol::make_object(state, sol::nil);
		auto t = value.as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setPosition table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setPosition(t["x"].get<float>(), t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScaleX", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setScaleX")) return sol::make_object(state, sol::nil);
		ccnode->setScaleX(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScaleY", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setScaleY")) return sol::make_object(state, sol::nil);
		ccnode->setScaleY(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setScale", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<sol::table>(value, "setScale")) return sol::make_object(state, sol::nil);
		auto t = value.as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setScale table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setScaleX(t["x"].get<float>());
		ccnode->setScaleY(t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPointX", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setAnchorPointX")) return sol::make_object(state, sol::nil);
		auto anchor = ccnode->getAnchorPoint();
		anchor.x = value.as<float>();
		ccnode->setAnchorPoint(anchor);
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPointY", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setAnchorPointY")) return sol::make_object(state, sol::nil);
		auto anchor = ccnode->getAnchorPoint();
		anchor.y = value.as<float>();
		ccnode->setAnchorPoint(anchor);
		return sol::make_object(state, sol::nil);
	});

	add("setAnchorPoint", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<sol::table>(value, "setAnchorPoint")) return sol::make_object(state, sol::nil);
		auto t = value.as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setAnchorPoint table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setAnchorPoint({ t["x"].get<float>(), t["y"].get<float>() });
		return sol::make_object(state, sol::nil);
	});

	add("setContentSizeWidth", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setContentSizeWidth")) return sol::make_object(state, sol::nil);
		auto size = ccnode->getContentSize();
		size.width = value.as<float>();
		ccnode->setContentSize(size);
		return sol::make_object(state, sol::nil);
	});

	add("setContentSizeHeight", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setContentSizeHeight")) return sol::make_object(state, sol::nil);
		auto size = ccnode->getContentSize();
		size.height = value.as<float>();
		ccnode->setContentSize(size);
		return sol::make_object(state, sol::nil);
	});

	add("setContentSize", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<sol::table>(value, "setContentSize")) return sol::make_object(state, sol::nil);
		auto t = value.as<sol::table>();

		if (!t["width"].is<float>() || !t["height"].is<float>()) {
			log::warn("[UI PLUGIN] setContentSize table missing float width/height.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setContentSize({ t["width"].get<float>(), t["height"].get<float>() });
		return sol::make_object(state, sol::nil);
	});

	add("setRotationX", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setRotationX")) return sol::make_object(state, sol::nil);
		ccnode->setRotationX(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setRotationY", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<float>(value, "setRotationY")) return sol::make_object(state, sol::nil);
		ccnode->setRotationY(value.as<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setRotation", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<sol::table>(value, "setRotation")) return sol::make_object(state, sol::nil);
		auto t = value.as<sol::table>();

		if (!t["x"].is<float>() || !t["y"].is<float>()) {
			log::warn("[UI PLUGIN] setRotation table missing float x/y.");
			return sol::make_object(state, sol::nil);
		}

		ccnode->setRotationX(t["x"].get<float>());
		ccnode->setRotationY(t["y"].get<float>());
		return sol::make_object(state, sol::nil);
	});

	add("setVisible", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<bool>(value, "setVisible")) return sol::make_object(state, sol::nil);
		ccnode->setVisible(value.as<bool>());
		return sol::make_object(state, sol::nil);
	});

	add("setID", [state](CCNode* ccnode, sol::object value) {
		if (!typeCheck<std::string>(value, "setID")) return sol::make_object(state, sol::nil);
		ccnode->setID(value.as<std::string>());
		return sol::make_object(state, sol::nil);
	});
}