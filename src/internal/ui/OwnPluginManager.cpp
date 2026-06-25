#include <internal/ui/OwnPluginManager.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

// this is obviously based on my PluginInfoPopup code

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
	backBtn->setID("arrow-back");

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
		->setGap(20)
	);

	md = MDTextArea::create("Loading...", {(layoutMenu->getContentSize().width / 2.0f) - 10.0f, layoutMenu->getContentSize().height - 15.0f});
	md->setID("description");
	layoutMenu->addChild(md);

	auto details = CCMenu::create();
	details->setID("details");
	details->setContentSize({(layoutMenu->getContentSize().width / 2.0f) - 10.0f, layoutMenu->getContentSize().height - 15.0f});
	details->setLayout(ColumnLayout::create()
		->setAxisReverse(true)
		->setAutoScale(false)
		->setGap(15)
	);

	auto top = CCMenu::create();
	top->setID("top");
	top->setContentSize({details->getContentWidth() + 2.2f, details->getContentHeight() / 2});
	top->setLayout(ColumnLayout::create()
		->setCrossAxisAlignment(AxisAlignment::Start)
		->setAutoScale(false)
		->setAxisReverse(true)
		->setGap(2.0f)
	);

	auto name = CCLabelBMFont::create("Plugin Uploader", "bigFont.fnt");
	name->setID("name");
	name->limitLabelWidth(top->getContentWidth(), 1.0f, 0.1f);
	top->addChild(name);

	auto breakLine = BreakLine::create(top->getContentWidth());
	top->addChild(breakLine);

	details->addChild(top);

	auto bottom = CCMenu::create();
	bottom->setID("bottom");
	bottom->setContentSize({details->getContentWidth(), (details->getContentHeight() / 2) + 10.0f});
	bottom->setLayout(ColumnLayout::create()
		->setAutoScale(false)
		->setGap(65)
	);

	auto textinputs = CCMenu::create();
	textinputs->setID("text-inputs");
	textinputs->setContentSize({bottom->getContentWidth(), bottom->getContentHeight() / 2});
	textinputs->setLayout(ColumnLayout::create()
		->setAxisReverse(true)
		->setAutoScale(false)
	);

	repoInput = TextInput::create(details->getContentWidth(), "Enter Owner/Repo");
	tagInput = TextInput::create(details->getContentWidth(), "Enter Release");

	textinputs->addChild(repoInput);
	textinputs->addChild(tagInput);

	auto upload = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Upload/Update", bottom->getContentWidth() - 15.0f, bottom->getContentWidth() - 15.0f, 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		this,
		menu_selector(OwnPluginManager::uploadPlugin)
	);
	upload->setID("upload-btn");

	bottom->addChild(upload);

	bottom->addChild(textinputs);

	details->addChild(bottom);

	layoutMenu->addChild(details);

	top->updateLayout();
	bottom->updateLayout();
	textinputs->updateLayout();
	details->updateLayout();
	layoutMenu->updateLayout();
	arrowMenu->updateLayout();

	m_mainLayer->addChild(layoutMenu);
	m_mainLayer->addChild(arrowMenu);

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

void OwnPluginManager::uploadPlugin(CCObject* sender) {
	
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