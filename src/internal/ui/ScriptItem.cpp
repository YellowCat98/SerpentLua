#include <internal/ui/ScriptItem.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal::ui;

bool ScriptItem::init(void* theMetadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, bool plugin) {
	if (!CCNode::init()) return false;
	this->plugin = plugin;

	if (plugin)
		this->metadata = static_cast<PluginMetadata*>(theMetadata);
	else
		this->metadata = static_cast<ScriptMetadata*>(theMetadata);


	this->setID(fmt::format("script-item/{}", METADATA_GET(id)));


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

	dev = CCLabelBMFont::create(METADATA_GET(developer).c_str(), "goldFont.fnt");

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

	titleLabel = CCLabelBMFont::create(METADATA_GET(name).c_str(), "bigFont.fnt");
	titleLabel->setID("title-label");
	titleLabel->setLayoutOptions(AxisLayoutOptions::create()->setScalePriority(1));
	title->addChild(titleLabel);

    versionLabel = CCLabelBMFont::create(METADATA_GET(version).c_str(), "bigFont.fnt");
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
	viewBtn->setID("toggle");
	// @geode-ignore(unknown-resource)
	auto errorBtn = CCMenuItemExt::createSpriteExtraWithFrameName("geode.loader/info-alert.png", 1.5f, [this](CCMenuItemSpriteExtra*) {
		std::string errorString = fmt::format("{}", fmt::join(std::get<ScriptMetadata*>(metadata)->errors, "\n"));
		MDPopup::create("Errors", errorString, "OK")->show();
	});
	errorBtn->setID("error-button");

	viewMenu->addChild(viewBtn);

	if (plugin) {
		errorBtn->setVisible(false);
	} else if (std::get<ScriptMetadata*>(metadata)->errors.empty()) errorBtn->setVisible(false);

	viewMenu->addChild(errorBtn);

	viewMenu->setLayout(
		RowLayout::create()
			->setAxisReverse(true)
			->setAxisAlignment(AxisAlignment::End)
			->setGap(10)
	);
	if (plugin) viewBtn->setVisible(false);

	this->addChild(bg);
	this->addChild(mainContainer);
	this->addChildAtPosition(viewMenu, Anchor::Right, ccp(-10, 0));
	mainContainer->updateLayout();
	title->updateLayout();
	devContainer->updateLayout();
	viewMenu->updateLayout();
	this->updateLayout();
	this->schedule(schedule_selector(ScriptItem::listener));
	return true;
}

// listens for changes and applies them
void ScriptItem::listener(float) {
	if (!plugin)
		this->viewBtn->toggle(Mod::get()->getSavedValue<bool>(fmt::format("enabled-{}", std::get<ScriptMetadata*>(metadata)->id)));
}
ScriptItem* ScriptItem::create(void* metadata, std::function<void(CCMenuItemToggler*)> onButton, const cocos2d::CCSize& size, bool plugin) {
    auto ret = new ScriptItem();
    if (ret && ret->init(metadata, onButton, size, plugin)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}