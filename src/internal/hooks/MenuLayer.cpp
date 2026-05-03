#include <Geode/Geode.hpp>
#include <SerpentLua.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <internal/SerpentLua.hpp>
#include <internal/ui/ScriptsLayer.hpp>
#include <argon/argon.hpp>

using namespace geode::prelude;
using namespace SerpentLua::internal;
using namespace SerpentLua;

class igiveup : public FLAlertLayerProtocol {
	void FLAlert_Clicked(FLAlertLayer*, bool btn2) override {
		if (btn2) {
			// user unfortunately doesnt want ot see this again
			Mod::get()->setSavedValue<bool>("show-missing-dll", true);
		}
	}
};

class $modify(SexyMenuLayer, MenuLayer) {
	struct Fields {
		async::TaskHolder<Result<std::string>> m_listener;
	};

	bool init() {
		if (!MenuLayer::init()) return false;

/*
		if (Mod::get()->getSettingValue<bool>("should-authenticate")) {
			m_fields->m_listener.spawn(
				argon::startAuth(),
				[](Result<std::string> res) {
					if (res.isOk()) {
						Notification::create("SerpentLua: Authentication succeeded", NotificationIcon::Success);
					} else {
						FLAlertLayer::create(
							"Authentication",
							fmt::format("An error occurred while authenticating with Argon: {}", res.unwrapErr()).c_str(),
							"OK"
						)->show();
					}
				}
			);
		}
*/

		static bool shownMissingWarning = std::filesystem::exists(Mod::get()->getConfigDir()/"plugin_global_deps"/"lua.dll");
		// assume the user has seen the warning beforehand if the file exists, so instead of checking for both std::filesystem::exists and if the user saw the warning, we only check one variable!
		if (Mod::get()->getSavedValue<bool>("show-missing-dll")) {
			if (!shownMissingWarning) shownMissingWarning = true;
		}

		if (!shownMissingWarning) {
			auto alert = FLAlertLayer::create(new (std::nothrow) igiveup(), "SerpentLua: Missing DLL",
				fmt::format(
					"<cb>lua.dll</c> was not found in the <cj>SerpentLua</c> config <cd>directory</c>.\n"
					"<cb>lua.dll</c> is <ca>essential</c> for <cp>plugins</c> to <cy>work</c>.\n"
					"<cp>Scripts</c> will load <cf>fine</c>, however they will <cr>not</c> be able to <cy>use</c> <cp>plugins</c>.\n"
					"Please install <cb>lua.dll</c> at the following <cd>path</c>:\n<cg>{}</c>\n",
					Mod::get()->getConfigDir()/"plugin_global_deps"/"lua.dll"
				).c_str(),
				"OK", "Dismiss", 400, true, 0, 1
			);

			auto tooManyColors = CCLabelBMFont::create("Did I color too many words? I think I colored too many words.", "chatFont.fnt");
			tooManyColors->setID("too-many-color"_spr);
			tooManyColors->setOpacity(51);
			tooManyColors->setScale(0.7f);

			auto alertSize = alert->m_mainLayer->getContentSize();

			tooManyColors->setAnchorPoint({0.5f, 0.0f});
			tooManyColors->setPosition({ alertSize.width / 2, 7.0f });

			alert->m_mainLayer->addChild(tooManyColors);

			alert->m_scrollingLayer->setStartOffset({0,0});

			alert->m_scene = this;
			alert->show();

			shownMissingWarning = true;
		}

		auto bottomMenu = static_cast<CCMenu*>(this->getChildByID("bottom-menu"));
		auto btn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("serpentluaButton.png"_spr), CircleBaseColor::Green, CircleBaseSize::MediumAlt), [](CCObject*) {
			CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, ui::ScriptsLayer::scene(false)));
		});
		btn->setID("serpentlua-btn"_spr);
		bottomMenu->addChild(btn);

		bottomMenu->updateLayout();

		return true;
	}
};