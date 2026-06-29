#include <internal/ui/PluginJudgmentPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginJudgmentPopup::init(const DisplayInfo& info) {
	if (!Popup::init({200.0f, 115.0f})) return false;
	this->setTitle("Plugin Review");

	auto actionsMenu = CCMenu::create();
	actionsMenu->setID("actions-menu");
	actionsMenu->setLayout(
		RowLayout::create()
		->setAxisAlignment(AxisAlignment::Even)
		->setCrossAxisAlignment(AxisAlignment::Center)
		->setCrossAxisLineAlignment(AxisAlignment::Center)
	);
	actionsMenu->setContentSize({m_size.width - 50.0f, m_size.height / 2});
	actionsMenu->setPosition({m_size.width / 2, (m_size.height / 2) - 15.0f});
	actionsMenu->setAnchorPoint({0.5f, 0.5f});

	auto approveSprite = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
	approveSprite->setScale(1.2f);
	auto approve = CCMenuItemSpriteExtra::create(approveSprite, this, menu_selector(PluginJudgmentPopup::startOperation));
	approve->setID("approve");
	approve->setTag(1);

	auto rejectSprite = CCSprite::createWithSpriteFrameName("GJ_dislikeBtn_001.png");
	rejectSprite->setScale(1.2f);
	auto reject = CCMenuItemSpriteExtra::create(rejectSprite, this, menu_selector(PluginJudgmentPopup::startOperation));
	reject->setID("reject");
	reject->setTag(0);

	actionsMenu->addChild(approve);
	actionsMenu->addChild(reject);

	actionsMenu->updateLayout();
	m_mainLayer->addChild(actionsMenu);

	return true;
}

void PluginJudgmentPopup::startOperation(CCObject* sender) {
	std::string op = sender->getTag() ? "approved" : "rejected";

	log::info("Shall be {}.", op);
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