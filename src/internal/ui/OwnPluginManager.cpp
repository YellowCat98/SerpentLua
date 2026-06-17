#include <internal/ui/OwnPluginManager.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool OwnPluginManager::init() {
	if (!Popup::init({300.0f, 200.0f})) return false;

	return true;
}

OwnPluginManager* OwnPluginManager::create() {
	auto ret = new OwnPluginManager();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}