#include <internal/ui/PluginInfoPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginInfoPopup::init(const DisplayInfo& info) {
	if (!Popup::init({300.0f, 200.0f})) return false;
	m_closeBtn->setID("close-btn");

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

	auto md = MDTextArea::create(info.description, {(layoutMenu->getContentSize().width / 2.0f) - 10.0f, layoutMenu->getContentSize().height - 15.0f});
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

	auto name = CCLabelBMFont::create(info.name.c_str(), info.featured ? "goldFont.fnt" : "bigFont.fnt");
	name->limitLabelWidth(top->getContentWidth(), 1.0f, 0.1f);

	auto nameBtn = CCMenuItemExt::createSpriteExtra(name, [info](CCMenuItemSpriteExtra*) {
		if (!info.featured) {
			FLAlertLayer::create("Hello.", "the.", "???")->show();
		} else {
			FLAlertLayer::create("Featured", "This plugin is <cy>featured</c>.", "OK")->show();
		}
	});
	nameBtn->setEnabled(info.featured);
	nameBtn->setID("name");

	top->addChild(nameBtn);



	auto dev = CCLabelBMFont::create(fmt::format("By {}", info.accountId).c_str(), "goldFont.fnt");
	dev->limitLabelWidth(top->getContentWidth() / 2, 0.5f, 0.1f);
	devBtn = CCMenuItemExt::createSpriteExtra(dev, [info](CCMenuItemSpriteExtra*) {
		ProfilePage::create(info.accountId, GJAccountManager::get()->m_accountID == info.accountId)->show();
	});
	devBtn->setID("developer");
	devBtn->setLayoutOptions(
		AxisLayoutOptions::create()
		->setNextGap(-2.0f)
	);
	devBtn->updateLayout();
	top->addChild(devBtn);

	auto breakLine = BreakLine::create(top->getContentWidth());
	top->addChild(breakLine);

	details->addChild(top);

	GameLevelManager::get()->m_userInfoDelegate = this;
	GameLevelManager::get()->getGJUserInfo(info.accountId);

	auto bottom = CCMenu::create();
	bottom->setID("bottom");
	bottom->setContentSize({details->getContentWidth(), (details->getContentHeight() / 2) + 10.0f});
	bottom->setLayout(ColumnLayout::create()
		->setAxisReverse(true)
		->setAutoScale(false)
		->setGap(65)
	);

	auto extras = CCMenu::create();
	extras->setID("extras");
	extras->setContentSize({bottom->getContentWidth(), bottom->getContentHeight() / 2});
	extras->setLayout(ColumnLayout::create()
		->setAxisReverse(true)
		->setAutoScale(false)
	);

	auto tpR = std::chrono::system_clock::time_point{
		std::chrono::seconds{info.releaseDate}
	};

	auto tpU = std::chrono::system_clock::time_point{
		std::chrono::seconds{info.lastUpdateDate}
	};

	auto release = utils::timeToAgoString(tpR);
	auto update = utils::timeToAgoString(tpU);
	
	auto releaseLabel = CCLabelBMFont::create(fmt::format("Released: {}", release).c_str(), "bigFont.fnt");
	releaseLabel->limitLabelWidth(details->getContentWidth() - 5.0f, 1.0f, 0.1f);
	auto updateLabel = CCLabelBMFont::create(fmt::format("Updated: {}", update).c_str(), "bigFont.fnt");
	updateLabel->limitLabelWidth(details->getContentWidth() - 5.0f, 1.0f, 0.1f);
	auto downloadLabel = CCLabelBMFont::create(fmt::format("Downloaded: {} time{}", info.downloadCount, info.downloadCount == 1 ? "" : "s").c_str(), "bigFont.fnt");
	downloadLabel->limitLabelWidth(details->getContentWidth() - 5.0f, 1.0f, 0.1f);

	extras->addChild(releaseLabel);
	extras->addChild(updateLabel);
	extras->addChild(downloadLabel);

	auto btns = CCMenu::create();
	btns->setID("buttons");
	btns->setContentSize({bottom->getContentWidth() - 15.0f, (bottom->getContentHeight() / 2) + 10.0f});
	btns->setLayout(ColumnLayout::create()
		->setAutoScale(false)
		->setAxisReverse(true)
	);

	auto miniBtnMenu = CCMenu::create();
	miniBtnMenu->setID("mini-btns");
	miniBtnMenu->setContentWidth(btns->getContentWidth());
	miniBtnMenu->setLayout(RowLayout::create());

	auto source = CCMenuItemExt::createSpriteExtraWithFrameName("geode.loader/source_generic.png", 0.8f, [info](CCMenuItemSpriteExtra*) {
		geode::createQuickPopup(
			"Confirm",
			fmt::format("Would you like to visit <cf>{}</c>?", info.source),
			"Cancel", "Confirm",
			[info](FLAlertLayer*, bool btn2) {
				if (btn2) web::openLinkInBrowser(info.source);
			}
		);
	});
	source->setID("source");

	if (ServerManager::get()->resolveStatus(ServerManager::Status::Staff)) {
		auto moderate = CCMenuItemExt::createSpriteExtraWithFilename("plugin_moderate.png"_spr, 0.8f, [info](CCMenuItemSpriteExtra*) {
			PluginJudgmentPopup::create(info)->show();
		});
		moderate->setID("moderate");

		miniBtnMenu->addChild(moderate);
	}

	miniBtnMenu->addChild(source);

	auto download = [this, info](bool script, CCMenuItemSpriteExtra* sender) {
		Notification::create("SerpentLua: Starting download...", NotificationIcon::Info)->show();
		sender->setEnabled(false);
		auto image = static_cast<ButtonSprite*>(sender->getNormalImage());
		image->updateBGImage("GJ_button_02.png");
		m_listener.spawn(std::move(ServerManager::get()->downloadPlugin(script, info, static_cast<ButtonSprite*>(sender->getNormalImage()))), [this, info, script, image, sender](geode::Result<geode::utils::web::WebResponse> resp) {
			image->updateBGImage("GJ_button_01.png");
			sender->setEnabled(true);
			static_cast<ButtonSprite*>(sender->getNormalImage())->setString("Installed");

			if (resp.isErr()) {
				auto alert = MDPopup::create("Error", resp.unwrapErr(), "OK");
				alert->m_scene = this;
				alert->show();
				return;
			} else {
				Notification::create(fmt::format("Downloaded {} plugin \"{}\" successfully!", script ? "script example for" : "", info.name), NotificationIcon::Success)->show();
				ScriptsLayer::changesMade();
			}
		});
	};

	auto downloadLovely = [this, info, download](bool script, CCMenuItemSpriteExtra* sender) {
		auto serpVerRes = VersionInfo::parse(info.serpentVersion);

		if (serpVerRes.isErr()) {
			FLAlertLayer::create("Error", fmt::format("Couldn't parse SerpentLua Version: {}", serpVerRes.unwrapErr()), "OK")->show();
			return;
		}

		auto serpVer = serpVerRes.unwrap();

		if (!utility::versionInfoCompare(Mod::get()->getVersion(), serpVer)) {
			FLAlertLayer::create("Cannot Install", fmt::format("This plugin is made for <cj>SerpentLua</c> version <cb>{}</c>\nYou are on <cy>{}</c>.", serpVer.toVString(), Mod::get()->getVersion().toVString()), "OK")->show();
			return;
		}

		if (info.status != "approved") {
			std::string warningText;
			if (info.status == "pending") warningText = "is currently under <cf>review</c>.";
			if (info.status == "rejected") warningText = "is <cr>rejected</c> from the index.\nThis plugin is not safe to install.";
			geode::createQuickPopup(
				"Warning",
				fmt::format(
					"This plugin {}\n"
					"Are you still sure you would like to install this plugin?",
					warningText
				),
				"Cancel", "Install",
				[download, sender, script](FLAlertLayer*, bool btn2) {
					if (btn2) {
						download(script, sender);
					}
				}
			);
		} else {
			download(script, sender);
		}
	};

	auto get = CCMenuItemExt::createSpriteExtra(
		ButtonSprite::create("Get", btns->getContentWidth(), btns->getContentWidth(), 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		[downloadLovely](CCMenuItemSpriteExtra* sender) {
			downloadLovely(false, sender);
		}
	);
	get->setID("get");
	auto getexple = CCMenuItemExt::createSpriteExtra(
		ButtonSprite::create("Get Example", btns->getContentWidth(), btns->getContentWidth(), 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		[downloadLovely](CCMenuItemSpriteExtra* sender) {
			downloadLovely(true, sender);
		}
	);
	if (info.scriptExample.empty()) {
		getexple->setEnabled(false);
		static_cast<ButtonSprite*>(getexple->getNormalImage())->setColor({127, 127, 127});
	}
	getexple->setID("get-example");

	btns->addChild(miniBtnMenu);
	btns->addChild(get);
	btns->addChild(getexple);

	bottom->addChild(extras);
	bottom->addChild(btns);

	details->addChild(bottom);

	layoutMenu->addChild(details);

	top->updateLayout();
	miniBtnMenu->updateLayout();
	bottom->updateLayout();
	extras->updateLayout();
	btns->updateLayout();
	details->updateLayout();
	layoutMenu->updateLayout();

	m_mainLayer->addChild(layoutMenu);

	auto ID = CCLabelBMFont::create(fmt::format("ID: {} | Version: {} | Made for SerpentLua {}", info.id, info.version, info.serpentVersion).c_str(), "chatFont.fnt");
	ID->setID("id");
	ID->setOpacity(200);
	ID->setScale(0.7f);
	ID->setAnchorPoint({0.5f, 0.0f});
	ID->setPosition({ m_size.width / 2, (m_mainLayer->getPositionY() - m_size.height) + 26.0f});
	m_mainLayer->addChild(ID);

	return true;
}

void PluginInfoPopup::getUserInfoFinished(GJUserScore* score) {
	auto baby = static_cast<CCLabelBMFont*>(devBtn->getNormalImage());
	baby->setString(fmt::format("By {}", score->m_userName).c_str());
}

void PluginInfoPopup::getUserInfoFailed(int id) {
	auto baby = static_cast<CCLabelBMFont*>(devBtn->getNormalImage());
	baby->setString("By ???");
	devBtn->setEnabled(false);
}

PluginInfoPopup* PluginInfoPopup::create(const DisplayInfo& info) {
	auto ret = new PluginInfoPopup();
	if (ret && ret->init(info)) {
		ret->autorelease();
		return ret;
	}
	delete ret;
	return nullptr;
}