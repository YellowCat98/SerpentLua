#include <internal/ui/PluginJudgmentPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginJudgmentPopup::init(const DisplayInfo& info) {
	if (!Popup::init({200.0f, 115.0f})) return false;
	m_closeBtn->setID("close-btn");
	this->setTitle("Plugin Review");
	this->info = info;
	this->featured = info.featured;

	auto featureSpriteON = CCSprite::createWithSpriteFrameName("GJ_sStarsIcon_001.png");
	featureSpriteON->setScale(1.8f);

	auto featureSpriteOFF = CCSprite::createWithSpriteFrameName("GJ_sStarsIcon_001.png");
	featureSpriteOFF->setScale(1.8f);
	featureSpriteOFF->setColor({127, 127, 127});

	feature = CCMenuItemExt::createToggler(featureSpriteON, featureSpriteOFF, [this](CCMenuItemToggler* sender) {
		featured = !featured;

		reject->setEnabled(!featured); // please dont feature rejected plugins
	});
	feature->toggle(featured);
	m_buttonMenu->addChildAtPosition(feature, Anchor::TopRight, {-1.0f, -3.67f});

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
	approve = CCMenuItemSpriteExtra::create(approveSprite, this, menu_selector(PluginJudgmentPopup::startOperation));
	approve->setID("approve");
	approve->setTag(1);

	auto rejectSprite = CCSprite::createWithSpriteFrameName("GJ_dislikeBtn_001.png");
	rejectSprite->setScale(1.2f);
	reject = CCMenuItemSpriteExtra::create(rejectSprite, this, menu_selector(PluginJudgmentPopup::startOperation));
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
	std::string fancy = sender->getTag() ? "<cg>approved</c>" : "<cr>rejected</c>";
	if (featured) fancy = "<cy>featured</c>";

	approve->setEnabled(false);
	reject->setEnabled(false);
	feature->setEnabled(false);

	auto req = ServerManager::get()->createReq(true);
	req.param("id", info.id);
	req.param("status", op);
	req.param("featured", static_cast<int>(featured));

	m_listener.spawn(std::move(ServerManager::get()->sendReq("PATCH", "/api/v1/moderator/set_plugin", req)), [fancy, this](web::WebResponse resp) {
		if (!resp.ok()) {
			MDPopup::create("Error", fmt::format("An error occurred: {}", resp.string().unwrap()), "OK")->show();
			return;
		}

		FLAlertLayer::create("Success!", fmt::format("Plugin {} has been {}!", info.name, fancy).c_str(), "OK")->show();
		this->onClose(nullptr);
	});
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