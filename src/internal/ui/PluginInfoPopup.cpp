#include "Geode/utils/web.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <internal/ui/PluginInfoPopup.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool PluginInfoPopup::init(const DisplayInfo& info) {
	if (!Popup::init({300.0f, 200.0f})) return false;

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
		->setCrossAxisAlignment(AxisAlignment::End)
		->setAutoScale(false)
		->setAxisReverse(true)
	);

	auto name = CCLabelBMFont::create(info.name.c_str(), "bigFont.fnt");
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

	auto gh = CCMenuItemExt::createSpriteExtraWithFrameName("geode.loader/github.png", 0.8f, [info](CCMenuItemSpriteExtra*) {
		web::openLinkInBrowser(info.source);
	});

	auto download = [this, info](bool script) {
		Notification::create("SerpentLua: Starting download...", NotificationIcon::Info)->show();
		ServerManager::get()->downloadPlugin(m_listener, script, info, [this, info, script](web::WebResponse resp, const std::string& err) {
			bool isErr = !err.empty();

			if (isErr) {
				auto alert = MDPopup::create("Error", err, "OK");
				alert->m_scene = this;
				alert->show();
			} else {
				Notification::create(fmt::format("Downloaded {} plugin \"{}\" successfully!", script ? "script example for" : "", info.name), NotificationIcon::Success)->show();
				ScriptsLayer::changesMade();
				onClose(nullptr); // geode does this when you start downloading idk why lol!
			}
		});
	};

	auto get = CCMenuItemExt::createSpriteExtra(
		ButtonSprite::create("Get", btns->getContentWidth(), btns->getContentWidth(), 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		[download](CCMenuItemSpriteExtra*) {
			download(false);
	});
	auto getexple = CCMenuItemExt::createSpriteExtra(
		ButtonSprite::create("Get Example", btns->getContentWidth(), btns->getContentWidth(), 1.0f, true, "goldFont.fnt", "GJ_button_01.png"),
		[download](CCMenuItemSpriteExtra*) {
			download(true);
		}
	);

	btns->addChild(gh);
	btns->addChild(get);
	btns->addChild(getexple);

	bottom->addChild(extras);
	bottom->addChild(btns);

	details->addChild(bottom);

	layoutMenu->addChild(details);

	top->updateLayout();
	bottom->updateLayout();
	extras->updateLayout();
	btns->updateLayout();
	details->updateLayout();
	layoutMenu->updateLayout();

	m_mainLayer->addChild(layoutMenu);

	auto ID = CCLabelBMFont::create(fmt::format("ID: {}", info.id).c_str(), "chatFont.fnt");
	ID->setID("id");
	ID->setOpacity(200);
	ID->setScale(0.7f);
	ID->setAnchorPoint({0.5f, 0.0f});
	ID->setPosition({ m_size.width / 2, (m_mainLayer->getPositionY() - m_size.height) + 26.0f});
	m_mainLayer->addChild(ID);

	return true;
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