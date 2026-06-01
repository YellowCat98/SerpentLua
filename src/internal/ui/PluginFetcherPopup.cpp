#include <internal/ui/PluginFetcherPopup.hpp>
#include <internal/ui/PluginInfoPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginFetcherPopup::init() {
	if (!Popup::init(333.0f, 130.0f)) return false;
	this->setTitle("Enter Plugin ID");
	m_closeBtn->setID("close-btn"); // close button deserves an id too!!!

	statusLabel = CCLabelBMFont::create("", "bigFont.fnt");
	statusLabel->setScale(0.5f);
	m_mainLayer->addChildAtPosition(statusLabel, Anchor::Bottom, {0.0f, -10.0f});

	auto infoBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 0.8f, [](CCMenuItemSpriteExtra*) {
		FLAlertLayer::create("Info", "The <co>plugin fetcher</c> allows you to get information about a plugin from the server chosen in the settings via ID.", "OK")->show();
	});
	infoBtn->setID("info-btn");
	m_buttonMenu->addChildAtPosition(infoBtn, Anchor::BottomRight, {-20.0f, 20.0f});

	textInput = TextInput::create(m_mainLayer->getContentWidth() - 15.0f, "plugin.id");
	textInput->setCommonFilter(CommonFilter::ID);
	textInput->setPosition({m_mainLayer->getContentWidth() / 2, (m_mainLayer->getContentHeight()/2) + 5.0f});
	m_mainLayer->addChild(textInput);

	auto ok = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("View"), [this](CCMenuItemSpriteExtra* sender) {
		sender->setEnabled(false);
		textInput->setEnabled(false);
		statusLabel->setString("Fetching...");
		statusLabel->setColor({255 , 255, 255});
		// begin da web requestino!
		auto req = ServerManager::get()->createReq(false);
		req.param("id", textInput->getString());

		Ref<PluginFetcherPopup> self = this;
		Ref<CCMenuItemSpriteExtra> betterSender = sender;

		listener.spawn(ServerManager::get()->sendReq("GET", "/api/v1/plugin/fetch", std::move(req)), [self = std::move(self), betterSender = std::move(betterSender)](web::WebResponse resp) {
			if (!(resp.code() >= 200 && resp.code() < 300)) {
				log::debug("error response");
				auto msg = fmt::format("Error: {} (Code {})", resp.string().unwrapOr("unknown error"), resp.code());
				// the server only returns strings in the case of an error (i promise ill make it better in the v2 api trust me)
				
				log::debug("queueinmainthread err");
				betterSender->setEnabled(true);
				self->statusLabel->setString(msg.c_str());
				self->statusLabel->setColor({255, 0, 0});
				self->textInput->setEnabled(true);
				return;
			}

			log::debug("no error getting json");

			auto json = resp.json().unwrapOr(matjson::Value()); // the fetch endpoint always returns a json in the case of a success
			
			PluginInfoPopup::create(DisplayInfo::create(json))->show();
			log::debug("success queueinmainthread");
			betterSender->setEnabled(true);
			self->statusLabel->setString("");
			self->textInput->setEnabled(true);
			self->textInput->setString("");
		});
	});
	m_buttonMenu->addChildAtPosition(ok, Anchor::Bottom, {0.0f, 25.0f});

	return true;
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