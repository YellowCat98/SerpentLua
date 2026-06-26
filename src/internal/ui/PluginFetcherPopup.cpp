#include <internal/ui/PluginFetcherPopup.hpp>
#include <internal/ui/PluginInfoPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginFetcherPopup::init() {
	if (!Popup::init(333.0f, 130.0f)) return false;
	this->setTitle("Enter Plugin ID");
	m_closeBtn->setID("close-btn"); // close button deserves an id too!!!

	statusLabel = CCLabelBMFont::create("", "bigFont.fnt");
	statusLabel->setID("status");
	statusLabel->setScale(0.5f);
	m_mainLayer->addChildAtPosition(statusLabel, Anchor::Bottom, {0.0f, -10.0f});

	auto infoBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 0.8f, [](CCMenuItemSpriteExtra*) {
		FLAlertLayer::create("Info", "The <co>plugin fetcher</c> allows you to get information about a plugin from the server chosen in the settings via ID.", "OK")->show();
	});
	infoBtn->setID("info-btn");
	m_buttonMenu->addChildAtPosition(infoBtn, Anchor::BottomRight, {-20.0f, 20.0f});

	textInput = TextInput::create(m_mainLayer->getContentWidth() - 15.0f, "plugin.id");
	textInput->setCommonFilter(CommonFilter::ID);
	textInput->setID("text-input");
	textInput->setPosition({m_mainLayer->getContentWidth() / 2, (m_mainLayer->getContentHeight()/2) + 5.0f});
	textInput->focus();
	m_mainLayer->addChild(textInput);

	auto ok = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("View"), [this](CCMenuItemSpriteExtra* sender) {
		sender->setEnabled(false);
		textInput->setEnabled(false);
		setStatusLabel("Fetching...");
		statusLabel->setColor({255 , 255, 255});
		// begin da web requestino!
		auto req = ServerManager::get()->createReq(false);
		req.param("id", textInput->getString());

		listener.spawn(std::move(ServerManager::get()->sendReq("GET", "/api/v1/plugin/fetch", std::move(req))), [this, sender](web::WebResponse resp) {
			sender->setEnabled(true);
			setStatusLabel("");
			this->textInput->setEnabled(true);
			if (!(resp.code() >= 200 && resp.code() < 300)) {
				auto msg = fmt::format("Error: {} (Code {})", resp.string().unwrapOr("unknown error"), resp.code());
				setStatusLabel(msg);
				this->statusLabel->setColor({255, 0, 0});
				return;
			}

			auto json = resp.json().unwrapOr(matjson::Value());
			
			PluginInfoPopup::create(DisplayInfo::create(json))->show();
			textInput->setString("");
			textInput->defocus();
		});
	});
	ok->setID("ok");
	m_buttonMenu->addChildAtPosition(ok, Anchor::Bottom, {0.0f, 25.0f});

	return true;
}

void PluginFetcherPopup::setStatusLabel(const std::string& text) {
	statusLabel->setString(text.c_str());
	statusLabel->limitLabelWidth(m_mainLayer->getContentWidth() + 50.0f, 0.5f, 0.01f);
}

PluginFetcherPopup* PluginFetcherPopup::create() {
	auto ret = new PluginFetcherPopup();
	if (ret && ret->init()) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}