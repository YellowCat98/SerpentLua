#include <internal/ui/PluginJudgmentPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginJudgmentPopup::init(const DisplayInfo& info) {
	if (!Popup::init({200.0f, 115.0f})) return false;

	return true;
}

PluginJudgmentPopup* PluginJudgmentPopup::create(const DisplayInfo& info) {
	auto ret = new PluginJudgmentPopup();
	if (ret && ret->init(info)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}