#include <internal/ui/SetUserStatusPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool SetUserStatusPopup::init(GJUserScore* score) {
	if (!Popup::init({200.0f, 115.0f})) return false;\
	m_closeBtn->setID("close-btn");
	this->setTitle(fmt::format("Set User Rank for {}", score->m_userName));
	statuses = {"Banned", "", "Verified", "Staff", "Admin", "Owner"};

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

	right = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(SetUserStatusPopup::movePage));
	right->setTag(1);
	right->getNormalImage()->setScaleX(-1.0f);
	right->setID("arrow-next");

	status = CCLabelBMFont::create("", "bigFont.fnt");
	status->setID("status");

	left = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(SetUserStatusPopup::movePage));
	left->setTag(-1);
	left->setID("arrow-back");

	actionsMenu->addChild(right);
	actionsMenu->addChild(left);

	return true;
}

void SetUserStatusPopup::movePage(CCObject* sender) {
	auto page = currentPage + sender->getTag();
	if (page == statuses.size()) return this->loadPage(1);
	if (page == 0) return this->loadPage(statuses.size());

	this->loadPage(currentPage + sender->getTag());
}

void SetUserStatusPopup::loadPage(int page) {
	currentPage = page - 1; // we are using vectors and i would like a super clear front


}