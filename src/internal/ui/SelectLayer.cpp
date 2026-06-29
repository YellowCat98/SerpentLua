#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/OwnPluginManager.hpp>
#include <internal/ui/SelectLayer.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal::ui;

bool SelectLayer::init() {
	if (!CCLayer::init()) return false;
	this->setKeypadEnabled(true);

	this->setID("SelectLayer"_spr);

	auto background = geode::createLayerBG();
	background->setID("background");
	background->setZOrder(-1);
	this->addChild(background);

	geode::addSideArt(this);

	auto backMenu = CCMenu::create();
	backMenu->setID("back-menu");

	auto backBtn = CCMenuItemExt::createSpriteExtra(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), [this](CCObject*) {
		this->keyBackClicked();
	});
	backMenu->addChild(backBtn);

	backMenu->setAnchorPoint({0.0f, 0.5f});

	backMenu->setLayout(SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Start)
		->setGap(5.0f)
	);
	this->addChildAtPosition(backMenu, Anchor::TopLeft, {12.0f, -25.0f}, false);

	buttonMenu = CCMenu::create();
	buttonMenu->setID("button-menu");

	infoMenu = CCMenu::create();
	infoMenu->setID("actions-menu");

	this->createButtons();

	buttonMenu->setLayout(
		RowLayout::create()
		->setGap(6)
		->setCrossAxisReverse(true)
		->setAxisAlignment(AxisAlignment::Center)
		->setCrossAxisAlignment(AxisAlignment::Center)
		->setCrossAxisLineAlignment(AxisAlignment::Center)
		->setGrowCrossAxis(true)
	);
	buttonMenu->updateLayout();

	infoMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisDirection(AxisDirection::RightToLeft)
		->setMainAxisAlignment(MainAxisAlignment::Start)
		->setCrossAxisAlignment(CrossAxisAlignment::End)
		->setCrossAxisScaling(AxisScaling::Fit)
		->setMainAxisScaling(AxisScaling::Fit)
	);
	infoMenu->updateLayout();


	this->addChild(buttonMenu);
	this->addChildAtPosition(infoMenu, Anchor::BottomLeft, {25.0f, 25.0f}, false);
	return true;
}

void SelectLayer::createButtons() {
	auto scriptsSpr = CategoryButtonSprite::createWithSpriteFrameName("script_select.png"_spr);
	auto scriptsBtn = CCMenuItemExt::createSpriteExtra(scriptsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(Source::Scripts)));
	});
	scriptsBtn->setID("scripts-btn");

	buttonMenu->addChild(scriptsBtn);

	auto pluginsSpr = CategoryButtonSprite::createWithSpriteFrameName("plugin_select.png"_spr);
	auto pluginsBtn = CCMenuItemExt::createSpriteExtra(pluginsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(Source::Plugins)));
	});
	pluginsBtn->setID("plugins-btn");

	buttonMenu->addChild(pluginsBtn);

	auto manageSpr = CategoryButtonSprite::createWithSpriteFrameName("manage_select.png"_spr);
	auto manageBtn = CCMenuItemExt::createSpriteExtra(manageSpr, [](CCMenuItemSpriteExtra*) {
		if (!ServerManager::get()->isAuthenticated()) {
			FLAlertLayer::create("Forbidden", "You must be <cb>authenticated</c> to access the <ca>plugin uploader</c>.", "OK")->show();
			return;
		} else if (ServerManager::get()->getStatusCached() == ServerManager::Status::Banned) {
			FLAlertLayer::create("Banned", fmt::format("You are <cr>banned</c> from uploading plugins.\n\n<cf>Reason:</c> {}", ServerManager::get()->getBanReason()).c_str(), "OK")->show();
			return;
		} else if (ServerManager::get()->getStatusCached() == ServerManager::Status::Unknown) {
			FLAlertLayer::create("Forbidden", "The <cb>SerpentLua</c> client could not determine your rank.\nPlease authenticate in the settings.\nIf this error keeps occurring, report this issue to the maintainers of the server you are using.", "OK")->show(); // this will be our lovely secret message because under normal circumstances you should never see this
			return;
		}

		OwnPluginManager::create()->show();
	});
	manageBtn->setID("manage-btn");
	buttonMenu->addChild(manageBtn);

	auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	auto infoBtn = CCMenuItemExt::createSpriteExtra(infoSpr, [](CCMenuItemSpriteExtra*) {
		FLAlertLayer::create(
			"Info",
			"This is where you may manage your install <cb>scripts</c> and <cb>plugins</c>.\n"
			"You may upload your own plugins or view information about them if you are a <cy>plugin developer</c>.",
			"OK"
		)->show();
	});
	infoBtn->setID("actions-btn");
	infoMenu->addChild(infoBtn);
}

void SelectLayer::keyBackClicked() {
	CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

SelectLayer* SelectLayer::create() {
	auto ret = new SelectLayer();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

CCScene* SelectLayer::scene() {
	auto ret = CCScene::create();
	ret->addChild(SelectLayer::create());
	return ret;
}