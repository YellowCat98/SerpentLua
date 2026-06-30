#include <internal/ui/SetUserStatusPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool SetUserStatusPopup::init(GJUserScore* score) {
	if (!Popup::init({200.0f, 115.0f})) return false;
	m_closeBtn->setID("close-btn");
	this->setTitle(fmt::format("Set Rank for {}", score->m_userName));
	statuses = ServerManager::get()->getStatusSettables();

	actionsMenu = CCMenu::create();
	actionsMenu->setID("actions-menu");
	actionsMenu->setLayout(
		RowLayout::create()
		->setCrossAxisAlignment(AxisAlignment::Center)
		->setCrossAxisLineAlignment(AxisAlignment::Center)
		->setAxisReverse(true)
		->setGap(5)
	);
	actionsMenu->setContentSize({m_size.width - 50.0f, m_size.height / 2});
	actionsMenu->setPosition({m_size.width / 2, (m_size.height / 2) + 5.0f});
	actionsMenu->setAnchorPoint({0.5f, 0.5f});

	right = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(SetUserStatusPopup::movePage));
	right->setTag(1);
	right->getNormalImage()->setScaleX(-1.0f);
	right->setID("arrow-next");

	status = CCLabelBMFont::create("Verified", "bigFont.fnt"); // verified is the longest one
	status->setID("status");

	left = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(SetUserStatusPopup::movePage));
	left->setTag(-1);
	left->setID("arrow-back");

	actionsMenu->addChild(right);
	actionsMenu->addChild(status);
	actionsMenu->addChild(left);

	actionsMenu->updateLayout();

	m_mainLayer->addChild(actionsMenu);

	loadPage(1);
	return true;
}

void SetUserStatusPopup::movePage(CCObject* sender) {
	auto page = currentPage + sender->getTag();
	if (page > statuses.size()) page = 1;
	if (page < 1) page = statuses.size();

	this->loadPage(page);
}

void SetUserStatusPopup::loadPage(int page) {
	currentPage = page;

	status->setString(ServerManager::get()->statusString(statuses[currentPage - 1]).c_str());
}

SetUserStatusPopup* SetUserStatusPopup::create(GJUserScore* score) {
	auto ret = new SetUserStatusPopup();
	if (ret && ret->init(score)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}