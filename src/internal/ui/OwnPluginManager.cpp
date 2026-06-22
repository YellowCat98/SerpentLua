#include <internal/ui/OwnPluginManager.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool OwnPluginManager::init() {
	if (!Popup::init({300.0f, 200.0f})) return false;

	// oh COME ON i dont want to do layout bs again

	auto arrowMenu = CCMenu::create();
	arrowMenu->setID("arrow-menu");
	arrowMenu->setContentSize({m_size.width + 100.0f, m_size.height});
	arrowMenu->setPosition(m_size / 2);
	arrowMenu->setLayout(
		SimpleRowLayout::create()
		->setMainAxisAlignment(MainAxisAlignment::Center)
		->setMainAxisDirection(AxisDirection::RightToLeft)
		->setGap(m_size.width + 20.0f)
	);

	nextBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(OwnPluginManager::movePage));
	nextBtn->setTag(1);
	nextBtn->getNormalImage()->setScaleX(-1.0f);
	nextBtn->setID("arrow-next");

	backBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), this, menu_selector(OwnPluginManager::movePage));
	backBtn->setTag(-1);
	backBtn->setID("arrow-next");

	arrowMenu->addChild(nextBtn);
	arrowMenu->addChild(backBtn);

	auto layoutMenu = CCMenu::create();
	layoutMenu->setID("layout-menu");
	layoutMenu->setContentSize(m_size);
	layoutMenu->setPosition(m_size / 2);
	layoutMenu->setLayout(RowLayout::create()
		->setAxisReverse(true)
		->setAxisAlignment(AxisAlignment::Center)
		->setAutoScale(false)
		->setGap(5)
	);

	md = MDTextArea::create("Loading...", {(layoutMenu->getContentSize().width / 2.0f) - 10.0f, layoutMenu->getContentSize().height - 15.0f});
	layoutMenu->addChild(md);

	auto details = CCMenu::create();
	details->setID("details");
	details->setContentSize({(layoutMenu->getContentSize().width / 2.0f) - 10.0f, layoutMenu->getContentSize().height - 15.0f});
	details->setLayout(ColumnLayout::create()
		->setAxisReverse(true)
		->setAutoScale(false)
		->setGap(15)
	);

	layoutMenu->addChild(details);

	arrowMenu->updateLayout();
	layoutMenu->updateLayout();
	details->updateLayout();

	m_mainLayer->addChild(arrowMenu);
	m_mainLayer->addChild(layoutMenu);

	loadPage(1);
	return true;
}

std::string OwnPluginManager::createPrettyPluginInfo(const DisplayInfo& info) {
	return fmt::format(
		"## {}\n"
		"- Status: {}\n"
		"- Download count: {}\n"
		"- Featured: {}",
		info.name,
		info.status,
		info.downloadCount,
		info.featured
	);
}

void OwnPluginManager::updateMD() {
	std::ostringstream ss;

	bool first = true;
	for (const auto& prettifiedString : prettyStrings) {
		if (!first) ss << "\n---\n";
		ss << prettifiedString;
		first = false;
	}

	this->md->setString(ss.str().c_str());
}

void OwnPluginManager::loadPage(int page) {
	currentPage = page;

	nextBtn->setVisible(false);
	backBtn->setVisible(false);

	this->prettyStrings = {};
	md->setString(fmt::format("Loading Page: {}", currentPage).c_str());

	auto req = ServerManager::get()->createReq();
	req.param("sort", "most_recent");
	req.param("account_id", 67);
	req.param("page", currentPage);

	m_listener.spawn(ServerManager::get()->sendReq("GET", "/api/v1/plugin/fetch/bulk", req), [this](web::WebResponse resp) {
		if (!(resp.code() >= 200 && resp.code() < 300)) {
			this->md->setString(fmt::format("Error: {} (Code {})", resp.string().unwrap(), resp.code()).c_str());
			return;
		}

		auto jsonRes = resp.json();
		if (jsonRes.isErr()) {
			this->md->setString(fmt::format("JSON error: {}", jsonRes.unwrapErr()).c_str());
			return;
		}

		auto json = jsonRes.unwrap();
		// there we go

		// first tings first we do da arrow stuff

		bool hasNext = json["has_next"].asBool().unwrap();
		bool hasPrev = json["has_prev"].asBool().unwrap();

		this->nextBtn->setVisible(hasNext);
		this->backBtn->setVisible(hasPrev);

		for (const auto& item : json["items"].asArray().unwrap()) {
			this->prettyStrings.push_back(this->createPrettyPluginInfo(DisplayInfo::create(item)));
		}
		this->updateMD();
	});
}

void OwnPluginManager::movePage(CCObject* sender) {
	this->loadPage(currentPage + sender->getTag());
}

OwnPluginManager* OwnPluginManager::create() {
	auto ret = new OwnPluginManager();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}