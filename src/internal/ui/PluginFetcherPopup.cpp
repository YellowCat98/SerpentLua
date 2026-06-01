#include <internal/ui/PluginFetcherPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginFetcherPopup::init() {
	if (!Popup::init(200.0f, 160.0f)) return false;

	this->setTitle("Plugin Fetcher");

	return true;
}

PluginFetcherPopup* PluginFetcherPopup::create() {
	auto ret = new PluginFetcherPopup();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}