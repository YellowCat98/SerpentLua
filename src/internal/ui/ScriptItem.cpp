#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <internal/ui/ScriptItem.hpp>
#include <internal/ui/PluginInfoPopup.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal::ui;

bool ScriptItem::init(const DisplayInfo& theMetadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, Source source) {
	if (!CCNode::init()) return false;
	this->source = source;

	std::optional<bool> current;

	this->metadata = theMetadata;

	if (metadata.script) {
		current = Mod::get()->getSavedValue<bool>(fmt::format("enabled-{}", metadata.id));
		this->lastState = *current; // guranteed to have a value
	}

	this->setID(fmt::format("script-item/{}", metadata.id));


	auto bg = CCScale9Sprite::create("GJ_square01.png");
	bg->setID("background");
	bg->setOpacity(0); // for some reason removing the bg entirely causes the ui to go BAD so i just set the opacity to 0
	bg->ignoreAnchorPointForPosition(false);
	bg->setAnchorPoint({0.5f, 0.5f});
	bg->setScale(0.7f);
	this->setContentSize(size);
	bg->setContentSize((m_obContentSize - ccp(6, 0)) / bg->getScale());
	bg->setPosition(m_obContentSize / 2);

	mainContainer = CCNode::create();
	mainContainer->setID("main-container");
	mainContainer->setScale(0.4f);
	mainContainer->setAnchorPoint({0.0f, 0.4f});
	mainContainer->setLayout(
		ColumnLayout::create()
			//->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::Even)
			->setCrossAxisLineAlignment(AxisAlignment::Start)
			->setGap(0)
	);

	title = CCNode::create();
	title->setID("title-container");
	title->setAnchorPoint({0.5f, 1.0f});
	title->setLayout(
		RowLayout::create()
			//->setDefaultScaleLimits(0.1f, 1.0f)
			->setAxisAlignment(AxisAlignment::Start)
	);

	devContainer = CCMenu::create();
	devContainer->setID("developer-container");
	devContainer->ignoreAnchorPointForPosition(false);
	devContainer->setAnchorPoint({0.0f, 0.0f});

	dev = CCLabelBMFont::create(metadata.developer.c_str(), "goldFont.fnt");

	devContainer->addChild(dev);

	devContainer->setLayout(
		RowLayout::create()
			->setAxisAlignment(AxisAlignment::Start)
	);

	mainContainer->addChild(devContainer);

	CCSize titleSpace {
		m_obContentSize.width / 2 - m_obContentSize.height,
		m_obContentSize.height + 10 - 5
	};

	mainContainer->addChild(title);

	titleLabel = CCLabelBMFont::create(metadata.name.c_str(), "bigFont.fnt");
	titleLabel->setID("title-label");
	titleLabel->setLayoutOptions(AxisLayoutOptions::create()->setScalePriority(1));
	title->addChild(titleLabel);

	versionLabel = CCLabelBMFont::create(metadata.version.c_str(), "bigFont.fnt");
	versionLabel->setID("version-label");
	versionLabel->setLayoutOptions(AxisLayoutOptions::create()
		->setScalePriority(1)
		->setAutoScale(false)
	);
	versionLabel->setScale(0.7f);
	versionLabel->setColor({0,255,255});
	title->addChild(versionLabel);

	title->setContentWidth((titleSpace.width) / mainContainer->getScale());

	devContainer->setContentWidth(titleSpace.width / mainContainer->getScale());

	mainContainer->setPosition(10, m_obContentSize.height / 2);
	mainContainer->setContentSize(ccp(titleSpace.width, titleSpace.height) / mainContainer->getScale());

	
	viewMenu = CCMenu::create();
	viewMenu->setID("view-menu");
	viewMenu->setAnchorPoint({1.0f, 0.5f});
	viewMenu->setScale(0.4f);

	viewBtn = CCMenuItemExt::createTogglerWithFrameName("GJ_checkOn_001.png", "GJ_checkOff_001.png", 1.5f, onButton);
	viewBtn->toggle(current.has_value() ? *current : false);
	viewBtn->setID("toggle");
	viewMenu->addChild(viewBtn);

	if (this->source == Source::Plugins || this->source == Source::Index) viewBtn->setVisible(false);

	auto deleteBtn = CCMenuItemExt::createSpriteExtra(
		ButtonSprite::create(
			CCSprite::createWithSpriteFrameName("edit_delBtn_001.png"),
			42, true, 40, "GJ_button_04.png", 1),
		[=, this](CCMenuItemSpriteExtra*) {
			if (this->source == Source::Plugins) if (!metadata.native) return;

			geode::createQuickPopup("Confirm",
				fmt::format("Are you sure you would like to <cr>delete</c> \"{}\"?\nThis action is <cr>irreversible</c>!", metadata.name),
				"Cancel", "Delete",
				[=, this](FLAlertLayer*, bool btn2) {
					if (btn2) {
						std::error_code ec;
						bool removed = std::filesystem::remove(metadata.path, ec);
						if (ec) {
							std::string message;
							if (ec.value() == 5) message = "<cr>This usually means a script is using this plugin.</c>";
							FLAlertLayer::create("Error", fmt::format("Error: \"{}\" (Code: {})\n{}", ec.message(), ec.value(), message).c_str(), "OK")->show();
						} else if (removed) {
							FLAlertLayer::create("Success", fmt::format("\"{}\" was removed <cg>successfully</c>!", metadata.name).c_str(), "OK")->show();
							ScriptsLayer::changesMade();
						}
					}
				}
			);
		});
	deleteBtn->setID("delete-button");

	if (this->source == Source::Plugins) deleteBtn->setVisible(metadata.native);
	else if (this->source == Source::Scripts) deleteBtn->setVisible(true);
	else deleteBtn->setVisible(false);

	viewMenu->addChild(deleteBtn);

	// @geode-ignore(unknown-resource)
	auto errorBtn = CCMenuItemExt::createSpriteExtraWithFrameName("geode.loader/info-alert.png", 1.5f, [this](CCMenuItemSpriteExtra*) {
		std::string errorString = fmt::format("{}", fmt::join(std::get<ScriptMetadata*>(metadata.internal)->errors, "\n"));
		MDPopup::create("Errors", errorString, "OK")->show();
	});
	errorBtn->setID("error-button");

	if (this->source != Source::Scripts) errorBtn->setVisible(false);
	else if (std::get<ScriptMetadata*>(metadata.internal)->errors.empty()) errorBtn->setVisible(false);

	viewMenu->addChild(errorBtn);

	auto infoBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 1.5f, [&](CCMenuItemSpriteExtra*) {
		PluginInfoPopup::create(metadata)->show();
	});
	infoBtn->setID("info-btn");
	if (this->source != Source::Index) infoBtn->setVisible(false);
	viewMenu->addChild(infoBtn);

	viewMenu->setLayout(
		RowLayout::create()
			->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::End)
			->setGap(10)
	);

	
	CCSize indicatorSize = {30, 30};

	auto indicatorContainer = CCMenu::create();
	//indicatorContainer->setContentSize(indicatorSize);
	indicatorContainer->setID("indicator-container");
	indicatorContainer->setPosition({
		m_obContentSize.width - 5, m_obContentSize.height - 3
	});
	indicatorContainer->ignoreAnchorPointForPosition(false);
	indicatorContainer->setAnchorPoint({1.0f, 1.0f});
	indicatorContainer->setScale(0.4f);
	indicatorContainer->setLayout(RowLayout::create()
		->setAxisAlignment(AxisAlignment::End)
		->setAxisReverse(true)
		->setGap(5)
	);

	//bool native = std::get<PluginMetadata*>(this->metadata)->native;

	// @geode-ignore(unknown-resource)
	auto indicatorSpr = CCSprite::createWithSpriteFrameName("geode.loader/updates-available.png");
	indicatorSpr->setScale(0.7f);
	indicatorSpr->setOpacity(100);
	auto indicator = CCMenuItemExt::createSpriteExtra(indicatorSpr, [](CCMenuItemSpriteExtra*) {
		FLAlertLayer::create("Plugin Indicator", "This is a <cf>non-native plugin</c>.", "OK")->show();
	});

	indicator->setID("native-indicator");
	indicator->setAnchorPoint({1.0f, 0.5f});
	indicatorContainer->addChild(indicator);
	if (this->source == Source::Plugins) indicator->setVisible(!metadata.native);
	else indicator->setVisible(false);

	this->addChild(bg);
	this->addChild(mainContainer);
	this->addChildAtPosition(viewMenu, Anchor::Right, ccp(-10, 0));
	this->addChild(indicatorContainer);
	mainContainer->updateLayout();
	title->updateLayout();
	devContainer->updateLayout();
	viewMenu->updateLayout();
	indicatorContainer->updateLayout();
	this->updateLayout();
	this->schedule(schedule_selector(ScriptItem::listener), 0.2f);
	return true;
}

// listens for changes and applies them
void ScriptItem::listener(float) {
	if (this->source == Source::Plugins) return;

	bool current = Mod::get()->getSavedValue<bool>(fmt::format("enabled-{}", metadata.id));

	if (lastState != current) {
		this->viewBtn->toggle(current);
		lastState = current;
		ScriptsLayer::changesMade();
	}
}

ScriptItem* ScriptItem::create(const DisplayInfo& metadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, Source source) {
	auto ret = new ScriptItem();
	if (ret && ret->init(metadata, onButton, size, source)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}