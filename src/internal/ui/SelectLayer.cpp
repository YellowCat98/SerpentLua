#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/PluginUploader.hpp>
#include <internal/ui/SelectLayer.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal::ui;

bool SelectLayer::init(bool adminPanel) {
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

	if (adminPanel && ServerManager::get()->isAuthenticated()) {
		createAdminPanel();
	} else {
		createPeasantPanel();
	}

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

void SelectLayer::createPeasantPanel() {
	auto scriptsSpr = CategoryButtonSprite::createWithSprite("script_select.png"_spr);
	auto scriptsBtn = CCMenuItemExt::createSpriteExtra(scriptsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(Source::Scripts)));
	});
	scriptsBtn->setID("scripts-btn");

	buttonMenu->addChild(scriptsBtn);

	auto pluginsSpr = CategoryButtonSprite::createWithSprite("plugin_select.png"_spr);
	auto pluginsBtn = CCMenuItemExt::createSpriteExtra(pluginsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(Source::Plugins)));
	});
	pluginsBtn->setID("plugins-btn");

	buttonMenu->addChild(pluginsBtn);

	auto safeSpr = CCSprite::createWithSpriteFrameName("GJ_safeBtn_001.png");
	safeSpr->setScale(0.8f);
	auto safeBtn = CCMenuItemExt::createSpriteExtra(safeSpr, [](CCMenuItemSpriteExtra*) {
		if (!ServerManager::get()->isAuthenticated()) {
			FLAlertLayer::create("Forbidden", "You must be <cb>authenticated</c> to enter the <ca>dashboard</c>.", "OK")->show();
			return;
		} else if (ServerManager::get()->getStatusCached() == ServerManager::Status::Banned) {
			FLAlertLayer::create("Banned", fmt::format("You are <cr>banned</c> from the dashboard.\n\n<cf>Reason:</c> {}", ServerManager::get()->getBanReason()).c_str(), "OK")->show();
			return;
		} else if (ServerManager::get()->getStatusCached() == ServerManager::Status::Unknown) {
			FLAlertLayer::create("Forbidden", "<cb>SerpentLua</c> could not determine your rank.", "OK")->show(); // this will be our lovely secret message because under normal circumstances you should never see this
			return;
		}

		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, SelectLayer::scene(true)));
	});
	safeBtn->setID("actions-btn");
	infoMenu->addChild(safeBtn);
}

void SelectLayer::createAdminPanel() {
	auto uploadSpr = CategoryButtonSprite::createWithSprite("upload_select.png"_spr);
	auto uploadBtn = CCMenuItemExt::createSpriteExtra(uploadSpr, [](CCMenuItemSpriteExtra*) {
		PluginUploader::create()->show();
	});
	buttonMenu->addChild(uploadBtn);

	auto validateSpr = CategoryButtonSprite::createWithSprite("validate_select.png"_spr);
	auto validateBtn = CCMenuItemExt::createSpriteExtra(validateSpr, [](CCMenuItemSpriteExtra*) {
		// i will define this later
	});
	buttonMenu->addChild(validateBtn);

	auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
	auto infoBtn = CCMenuItemExt::createSpriteExtra(infoSpr, [this](CCMenuItemSpriteExtra*) {
		auto alert = FLAlertLayer::create(
			"Info",
			"Welcome to the <ca>dashboard</c>!\n"
			"You may upload or publish plugins here. More tools may be unlocked if you have permission.\n",
			"OK"
		);
		alert->m_scene = this;
		alert->show();
	});
	infoBtn->setID("actions-btn");
	infoMenu->addChild(infoBtn);

	if (!Mod::get()->setSavedValue("shown-dashboard-info", true)) {
		infoBtn->activate();
	}
}

void SelectLayer::keyBackClicked() {
	CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

SelectLayer* SelectLayer::create(bool adminPanel) {
	auto ret = new SelectLayer();
	if (ret && ret->init(adminPanel)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}

CCScene* SelectLayer::scene(bool adminPanel) {
	auto ret = CCScene::create();
	ret->addChild(SelectLayer::create(adminPanel));
	return ret;
}