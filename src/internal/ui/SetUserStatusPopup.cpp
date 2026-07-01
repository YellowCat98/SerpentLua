#include <internal/ui/SetUserStatusPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool SetUserStatusPopup::init(GJUserScore* score) {
	if (!Popup::init({225.0f, 150.0f})) return false;
	m_closeBtn->setID("close-btn");
	this->setTitle(fmt::format("Set Rank for {}", score->m_userName));
	this->statuses = ServerManager::get()->getStatusSettables();
	this->score = score;

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
	actionsMenu->setPosition({m_size.width / 2, (m_size.height / 2) + 20.0f});
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

	textInput = TextInput::create(actionsMenu->getContentWidth(), "Enter Ban Reason");
	textInput->setPosition({actionsMenu->getPositionX(), actionsMenu->getPositionY() - 35.0f});

	m_mainLayer->addChild(textInput);

	auto applySpr = ButtonSprite::create("Apply");
	apply = CCMenuItemSpriteExtra::create(applySpr, this, menu_selector(SetUserStatusPopup::startOperation));

	m_buttonMenu->addChildAtPosition(apply, Anchor::Bottom, {0.0f, 25.0f});

	actionsMenu->updateLayout();

	m_mainLayer->addChild(actionsMenu);

	loadPage(1);
	return true;
}

void SetUserStatusPopup::movePage(CCObject* sender) {
	auto page = currentPage + sender->getTag();
	if (page > statuses.size()) page = 1;
	else if (page < 1) page = statuses.size();

	this->loadPage(page);
}

void SetUserStatusPopup::loadPage(int page) {
	currentPage = page;
	currentIndex = currentPage - 1;

	currentStatus = statuses[currentIndex];
	textInput->setVisible(currentStatus == ServerManager::Status::Banned);

	status->setString(ServerManager::get()->statusString(currentStatus).c_str());
}

void SetUserStatusPopup::startOperation(CCObject*) {
	left->setEnabled(false);
	right->setEnabled(false);
	apply->setEnabled(false);
	textInput->setEnabled(false);

	auto req = ServerManager::get()->createReq(true);
	req.param("status", ServerManager::get()->statusString(statuses[currentIndex], true));
	req.param("account_id", score->m_accountID);
	if (currentStatus == ServerManager::Status::Banned) req.param("ban_reason", textInput->getString());

	m_listener.spawn(std::move(ServerManager::get()->sendReq("PATCH", "/api/v1/moderator/set_user_status", req)), [this](web::WebResponse resp) {
		if (!resp.ok()) {
			MDPopup::create("Error", fmt::format("An error occurred: {}", resp.string().unwrap()), "OK")->show();
			this->onClose(nullptr);
			return;
		}

		FLAlertLayer::create("Success", fmt::format("Set status <cg>{}</c> for user <cy>{}</c>", ServerManager::get()->statusString(statuses[currentIndex]), score->m_userName).c_str(), "OK")->show();
		this->onClose(nullptr);
	});
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