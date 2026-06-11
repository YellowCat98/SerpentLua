#include <internal/ui/PluginUploader.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginUploader::init() {
	if (!Popup::init({300.0f, 200.0f})) return false;

	return true;
}

PluginUploader* PluginUploader::create() {
	auto ret = new PluginUploader();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}