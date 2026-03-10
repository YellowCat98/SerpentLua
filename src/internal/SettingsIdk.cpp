#include <internal/SerpentLua.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal;

geode::Result<std::shared_ptr<SettingV3>> OpenScriptsSettingV3::parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
	auto res = std::make_shared<OpenScriptsSettingV3>();
	auto root = checkJson(json, "OpenScriptsSettingV3");

	res->init(key, modID, root);
	res->parseNameAndDescription(root);
	res->parseEnableIf(root);

	root.checkUnknownKeys();
	return root.ok(std::static_pointer_cast<SettingV3>(res));
}

bool OpenScriptsSettingV3::load(matjson::Value const& json) {
	return true;
}

bool OpenScriptsSettingV3::save(matjson::Value& json) const {
	return true;
}

bool OpenScriptsSettingV3::isDefaultValue() const {
	return true;
}

void OpenScriptsSettingV3::reset() {}

SettingNodeV3* OpenScriptsSettingV3::createNode(float width) {
	return OpenScriptsSettingNodeV3::create(
		std::static_pointer_cast<OpenScriptsSettingV3>(shared_from_this()),
		width
	);
}

// took notes from:
// https://github.com/geode-sdk/geode/blob/551a1b62100a966334bae1abe826aa40fca93578/loader/src/loader/CustomGeodeSettings.cpp#L115-L155
bool OpenScriptsSettingNodeV3::init(std::shared_ptr<OpenScriptsSettingV3> setting, float width) {
	if (!SettingNodeV3::init(setting, width)) return false;

	this->getNameLabel()->setVisible(false);

	auto openScriptsSpr = ButtonSprite::create("Open Scripts Folder", "goldFont.fnt", "GJ_button_01.png");
	openScriptsSpr->setScale(0.5f);

	auto openScriptsBtn = CCMenuItemExt::createSpriteExtra(openScriptsSpr, [](CCMenuItemSpriteExtra* sender) {
		utils::file::openFolder(Mod::get()->getConfigDir() / "scripts");
	});
	this->getButtonMenu()->addChild(openScriptsBtn);

	auto openPluginsSpr = ButtonSprite::create("Open Plugins Folder", "goldFont.fnt", "GJ_button_01.png");
	openPluginsSpr->setScale(0.5f);

	auto openPluginsBtn = CCMenuItemExt::createSpriteExtra(openPluginsSpr, [](CCMenuItemSpriteExtra* sender) {
		utils::file::openFolder(Mod::get()->getConfigDir() / "plugins");
	});
	this->getButtonMenu()->addChild(openPluginsBtn);

	this->getButtonMenu()->setPosition(this->getContentSize() / 2);
	this->getButtonMenu()->setAnchorPoint({0.5f, 0.5f});
	this->getButtonMenu()->setContentWidth(width - 20);
	this->getButtonMenu()->setLayout(RowLayout::create());

	this->updateState(nullptr);
	return true;
}

void OpenScriptsSettingNodeV3::updateState(CCNode* invoker) {
	SettingNodeV3::updateState(invoker);
}

void OpenScriptsSettingNodeV3::onCommit() {}
void OpenScriptsSettingNodeV3::onResetToDefault() {}

OpenScriptsSettingNodeV3* OpenScriptsSettingNodeV3::create(std::shared_ptr<OpenScriptsSettingV3> setting, float width) {
	auto ret = new OpenScriptsSettingNodeV3();
	if (ret->init(setting, width)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

bool OpenScriptsSettingNodeV3::hasUncommittedChanges() const {
	return false;
}

bool OpenScriptsSettingNodeV3::hasNonDefaultValue() const {
	return false;
}