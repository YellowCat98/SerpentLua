#include "Geode/ui/General.hpp"
#include "Geode/ui/Layout.hpp"
#include "ui/ScriptsLayer.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
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

	auto buttonMenu = CCMenu::create();

	auto scriptsSpr = CategoryButtonSprite::createWithSprite("script_select.png"_spr);

	auto scriptsBtn = CCMenuItemExt::createSpriteExtra(scriptsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(false)));
	});

	buttonMenu->addChild(scriptsBtn);

	auto pluginsSpr = CategoryButtonSprite::createWithSprite("plugin_select.png"_spr);

	auto pluginsBtn = CCMenuItemExt::createSpriteExtra(pluginsSpr, [](CCMenuItemSpriteExtra* sender) {
		CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(true)));
	});

	buttonMenu->addChild(pluginsBtn);

	auto indexSpr = CategoryButtonSprite::createWithSprite("index_select.png"_spr);

	auto indexBtn = CCMenuItemExt::createSpriteExtra(indexSpr, [](CCMenuItemSpriteExtra* sender) {
		log::info("Feet!");
	});

	buttonMenu->addChild(indexBtn);

	buttonMenu->setLayout(
		RowLayout::create()
		->setGap(6)
		->setCrossAxisReverse(true)
		->setAxisAlignment(AxisAlignment::Center)
		->setCrossAxisAlignment(AxisAlignment::Center)
		->setCrossAxisLineAlignment(AxisAlignment::Center)
		->setGrowCrossAxis(true)
	);

	buttonMenu->setContentWidth(scriptsBtn->getContentWidth() * 3 + 12);
	buttonMenu->updateLayout();
	
	this->addChild(buttonMenu);
	return true;
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