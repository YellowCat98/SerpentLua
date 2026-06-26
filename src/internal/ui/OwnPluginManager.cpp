#include <internal/ui/OwnPluginManager.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

// this is obviously based on my PluginInfoPopup code

bool OwnPluginManager::init() {
	if (!Popup::init({300.0f, 200.0f})) return false;

	// oh COME ON i dont want to do layout bs again

	statusLabel = CCLabelBMFont::create("", "bigFont.fnt");
	statusLabel->setID("status");
	statusLabel->setScale(0.5f);
	m_mainLayer->addChildAtPosition(statusLabel, Anchor::Bottom, {0.0f, -10.0f});

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

	repoInput = TextInput::create(details->getContentWidth(), "Enter Repo");
	repoInput->setCommonFilter(CommonFilter::Any);
	tagInput = TextInput::create(details->getContentWidth(), "Enter Release");
	tagInput->setCommonFilter(CommonFilter::Any);

	textinputs->addChild(repoInput);
	textinputs->addChild(tagInput);

	uploadBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Upload/Update", bottom->getContentWidth() - 15.0f, bottom->getContentWidth() - 15.0f, 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		this,
		menu_selector(OwnPluginManager::beginUpload)
	);
	uploadBtn->setID("upload-btn");

	bottom->addChild(uploadBtn);

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

void OwnPluginManager::setStatusLabel(const std::string& text) {
	statusLabel->setString(text.c_str());
	statusLabel->limitLabelWidth(m_mainLayer->getContentWidth() + 50.0f, 0.5f, 0.01f);
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
	req.param("account_id", GJAccountManager::get()->m_accountID);
	req.param("page", currentPage);

	m_listener.spawn(std::move(ServerManager::get()->sendReq("GET", "/api/v1/plugin/fetch/bulk", req)), [this](web::WebResponse resp) {
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

void OwnPluginManager::beginUpload(CCObject* sender) {
	repoInput->setEnabled(false);
	tagInput->setEnabled(false);
	uploadBtn->setEnabled(false);
	this->setStatusLabel("Getting index.json...");

	std::string repo = repoInput->getString();
	std::string tag = tagInput->getString();

	m_beginListener.spawn(std::move(ServerManager::get()->getIndexJSON(repo, tag)), [this, repo, tag](geode::Result<matjson::Value> resp) {
		if (resp.isErr()) {
			MDPopup::create("Error", resp.unwrapErr(), "OK")->show();
			onFinishedUploading();
			return;
		}

		auto json = resp.unwrap();

		// ok i have to check every single thing i access in that url please wish me luck

		auto idRes = json["owned_by"].asInt();
		if (idRes.isErr()) {
			MDPopup::create("Error", fmt::format("Unable to get accountID from index.json: {}", idRes.unwrapErr()), "OK")->show();
			onFinishedUploading();
			return;
		}

		int id = idRes.unwrap();
		if (GJAccountManager::get()->m_accountID != id) {
			MDPopup::create("Error", "You don't own this plugin.", "OK")->show();
			onFinishedUploading();
			return;
		}

		// first ill just verify if they all exist or not
		auto pluginRes = json["plugin"].asString();
		if (pluginRes.isErr()) {
			MDPopup::create("Error", fmt::format("Unable to get plugin from index.json: {}", pluginRes.unwrapErr()), "OK")->show();
			onFinishedUploading();
			return;
		}

		auto descRes = json["desc"].asString();
		if (descRes.isErr()) {
			MDPopup::create("Error", fmt::format("Unable to get description from index.json: {}", descRes.unwrapErr()), "OK")->show();
			onFinishedUploading();
			return;
		}

		this->setStatusLabel("Got index.json, creating request body.");

		setDownloadLinks(json, repo, tag);
	});
}

void OwnPluginManager::setDownloadLinks(const matjson::Value& indexJson, const std::string& repo, const std::string& tag) {
	body["download_link"] = constructUrl(repo, tag, indexJson["plugin"].asString().unwrap());

	auto example = indexJson["example"].asString();
	if (example.isOk()) {
		body["script_example"] = constructUrl(repo, tag, example.unwrap());
	}

	setDescription(indexJson, repo, tag);
}

void OwnPluginManager::setDescription(const matjson::Value& indexJson, const std::string& repo, const std::string& tag) {
	this->setStatusLabel("Getting/Setting description...");

	auto req = web::WebRequest();
	
	auto descUrl = constructUrl(repo, tag, indexJson["desc"].asString().unwrap());

	m_bodyListener.spawn(std::move(req.get(descUrl)), [this, indexJson, repo, tag](web::WebResponse resp) {
		if (!resp.ok()) {
			MDPopup::create("Error", fmt::format("Unable to download file (Code {})", resp.code()), "OK")->show();
			onFinishedUploading();
			return;
		}

		this->body["description"] = resp.string().unwrap();

		this->setBasicMetadata(indexJson, repo, tag);

	});
}

void OwnPluginManager::setBasicMetadata(const matjson::Value& indexJson, const std::string& repo, const std::string& tag) {
	this->setStatusLabel("Setting metadata...");

	auto pluginUrl = constructUrl(repo, tag, indexJson["plugin"].asString().unwrap());

	auto callIfReady = [this, indexJson, repo, tag]() {
		if (pluginMetaFinished && scriptHashFinished) {
			this->uploadOrUpdate(indexJson, repo, tag);
		}
	};

	m_pluginMetaListener.spawn(std::move(ServerManager::get()->getPluginMetadataByUrl(pluginUrl)), [this, repo, callIfReady](geode::Result<std::pair<PluginMetadata*, geode::utils::web::WebResponse>> pairRes) {
		if (pairRes.isErr()) {
			MDPopup::create("Error", fmt::format("Unable to get plugin metadata: {}", pairRes.unwrapErr()), "OK")->show();
			onFinishedUploading();
			return;
		}
		auto pair = pairRes.unwrap();

		auto mdPlugin = pair.first;
		auto resp = pair.second;

		auto hash = ServerManager::get()->getDownloadHashByData(resp.data());

		body["name"] = mdPlugin->name;
		body["id"] = mdPlugin->id;
		body["version"] = mdPlugin->version;
		body["serpent_version"] = mdPlugin->serpentVersion;
		body["download_hash"] = hash;
		body["source"] = fmt::format("https://github.com/{}", repo);

		pluginMetaFinished = true;
		callIfReady();
	});

	auto scriptUrlRes = indexJson["example"].asString();

	if (scriptUrlRes.isOk()) {
		auto scriptUrl = constructUrl(repo, tag, indexJson["example"].asString().unwrap());
		m_scriptHashListener.spawn(std::move(ServerManager::get()->getDownloadHashByUrl(scriptUrl)), [this, callIfReady](geode::Result<std::string> res) {
			if (res.isErr()) {
				MDPopup::create("Error", fmt::format("Unable to get script download hash: {}", res.unwrapErr()), "OK")->show();
				onFinishedUploading();
				return;
			}

			body["script_download_hash"] = res.unwrap();

			scriptHashFinished = true;
			callIfReady();
		});
	} else {
		scriptHashFinished = true;
	}
}

void OwnPluginManager::uploadOrUpdate(const matjson::Value& indexJson, const std::string& repo, const std::string& tag) {
	log::debug("Starting request with body:\n{}", body.dump());
	this->setStatusLabel("Determining update or upload...");

	auto req = ServerManager::get()->createReq();
	req.param("id", body["id"].asString().unwrap());

	m_bodyListener.spawn(std::move(ServerManager::get()->sendReq("GET", "/api/v1/plugin/exists", req)), [this](web::WebResponse resp) {
		if (!resp.ok()) {
			MDPopup::create("Error", fmt::format("Unable to verify upload or update: {}", resp.string().unwrap()), "OK")->show();
			onFinishedUploading();
			return;
		}

		auto existsRes = utils::numFromString<int>(resp.string().unwrap());
		if (existsRes.isErr()) {
			MDPopup::create("Error", existsRes.unwrapErr(), "OK")->show();
			onFinishedUploading();
			return;
		}

		bool exists = existsRes.unwrap();
		std::string endpoint = exists ? "update" : "publish";
		std::string epPrettified = exists ? "Updating" : "Publishing";
		std::string method = exists ? "PATCH" : "POST";
		
		this->setStatusLabel(fmt::format("{} Plugin...", epPrettified));

		auto newReq = ServerManager::get()->createReq(true); // the FIRST ever withAuth = true createReq call EVER someone document this
		newReq.bodyJSON(body);

		auto url = fmt::format("/api/v1/plugin/{}", endpoint);

		m_bodyListener.spawn(std::move(ServerManager::get()->sendReq(method, url, newReq)), [this, exists](web::WebResponse resp) {
			if (!resp.ok()) {
				MDPopup::create("Error", fmt::format("Unable to upload/update plugin: {}", resp.string().unwrap()), "OK")->show();
				onFinishedUploading();
				return;
			}

			FLAlertLayer::create(fmt::format("{}!", exists ? "Updated!" : "Uploaded!").c_str(), fmt::format("Your plugin \"{}\" has been {} <cg>successfully!</c>", body["name"].asString().unwrap(), exists ? "updated" : "uploaded").c_str(), "OK")->show();
			onFinishedUploading();
		});
	});
}

std::string OwnPluginManager::constructUrl(std::string repo, std::string tag, std::string filename) {
	return fmt::format("https://github.com/{}/releases/download/{}/{}", repo, tag, filename);
}

void OwnPluginManager::onFinishedUploading() {
	repoInput->setEnabled(true);
	tagInput->setEnabled(true);
	uploadBtn->setEnabled(true);
	setStatusLabel("");
	body = {};
	pluginMetaFinished = false;
	scriptHashFinished = false;
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