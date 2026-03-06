#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>
#include <Geode/utils/async.hpp>
#include <Geode/ui/GeodeUI.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

void ScriptsLayer::refreshWith(CCArray* array) {
    this->m_scriptsListLayer->m_listView->removeFromParent();

    this->m_scriptsListLayer->m_listView = ListView::create(array, 30.0f);
    m_scriptsListLayer->addChild(m_scriptsListLayer->m_listView);
}

void ScriptsLayer::loadPage(int page) {
    //auto allScripts = internal::RuntimeManager::get()->getAllScripts();

    std::vector<void*> scriptsInPage;

    int start = (page - 1) * itemsPerPage; // reason we do page - 1 is because indexes start at 0
    int end = start + itemsPerPage - 1; // pages overlap when you dont do the -1
    int idx = 0;

    for (auto it = scripts.begin(); it != scripts.end(); ++it, ++idx) {
        if (idx >= start && idx <= end) {
            scriptsInPage.push_back((*it).second); // i have to do (*it).second otherwise itll parse it as *(it.second)
        }
    }


    CCArray* array = CCArray::create(); // in this case, using CCArrayExt doesnt really make much of a difference cause literally all i do is push back, and listView handles everything.
    for (auto& item : scriptsInPage) {
        array->addObject(ScriptItem::create(item, [](CCMenuItemToggler* button) {
            auto item = typeinfo_cast<ScriptItem*>(button->getParent()->getParent()); // normally this should never return nullptr, but just in case!
            if (!item) return;
            auto metadata = item->metadata;
            if (std::holds_alternative<PluginMetadata*>(metadata)) {
                Notification::create("You cannot enable/disable a plugin!")->show();
                return; // even though by default you shouldnt be able to reach this button, you can make it visible and clickable through devtools, which is bad
            }
            Mod::get()->setSavedValue<bool>(fmt::format("enabled-{}", std::get<ScriptMetadata*>(metadata)->id), !button->isToggled());
        }, CCSize(358.0f, 30), plugin));
    }

    backBtn->setVisible(true);
    nextBtn->setVisible(true);
    
    if (!scriptsInPage.empty()) {
        if (scriptsInPage.front() == scripts.begin()->second) { // basically if were on the first page
            backBtn->setVisible(false);
        }

        if (scriptsInPage.back() == scripts.rbegin()->second) { // basically if were on the last page
            nextBtn->setVisible(false);
        }
    } else {
        backBtn->setVisible(false);
        nextBtn->setVisible(false);
    }


    this->refreshWith(array);

    this->currentPage = page;

    infoLabel->setString(fmt::format("Page {}/{} ({} Items)", currentPage, (scripts.size() + itemsPerPage - 1) / itemsPerPage, scripts.size()).c_str());

}


void ScriptsLayer::setupScriptsList() {
    CCArray* scriptItems = CCArray::create();
    
    
    if (!plugin) {
        for (const auto [k, v] : RuntimeManager::get()->getAllScripts()) {
            scripts[k] = static_cast<void*>(v);
        }
    } else {
        for (const auto [k, v] : RuntimeManager::get()->getAllLoadedPlugins()) {
            scripts[k] = static_cast<void*>(v->metadata);
        }
    }

/*
    for (auto& script : SerpentLua::internal::RuntimeManager::get()->getAllScripts()) {
        scriptItems->addObject(ScriptItem::create(script.second, [](CCMenuItemToggler* button) {
            auto item = typeinfo_cast<ScriptItem*>(button->getParent()->getParent()); // normally this should never return nullptr, but just in case!
            if (!item) return;
            auto metadata = item->metadata;
            Mod::get()->setSavedValue<bool>(fmt::format("enabled-{}", metadata->id), !button->isToggled());
        }, CCSize(358.0f, 30)));
    }
*/
    

/*
    for (int i = 0; i < 81; i++) {
        std::map<std::string, std::string> metadatamap = {
            {"name", fmt::format("test, {}", i)},
            {"developer", "hi"},
            {"id", fmt::format("oy_{:02}", i)},
            {"version", "1.0.0"},
            {"serpent-version", "yoo"},
            {"plugins", "hi hi hi hi"}
        };

        scripts.insert({metadatamap["id"], ScriptMetadata::create(metadatamap)});
    }
*/

    auto listView = ListView::create(CCArray::create(), 30);
    
    m_scriptsListLayer = GJListLayer::create(listView, plugin ? "plugins" : "scripts", {194, 114, 62, 255}, 358.0f, 220.0f, 0);
    m_scriptsListLayer->setID("script-list"); // normally i WOULD make it so it would be plugins-list if it was a plugin but eh it would be too wonky
    auto listlayerSize = m_scriptsListLayer->getContentSize();
    m_scriptsListLayer->setPosition(ccp((winSize.width - listlayerSize.width)/2, (winSize.height - listlayerSize.height)/2));

    this->addChild(m_scriptsListLayer);

    auto arrowMenu = CCMenu::create(); // luckily, positioning a ccmenu at 0, 0 makes it fill the entire screen! meaning i can just do some layout stuff to get these working!
    arrowMenu->setID("arrow-menu");

    nextBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"), this, menu_selector(ScriptsLayer::callbackMovePage));
    nextBtn->setTag(1);
    nextBtn->setID("arrow-next");
    nextBtn->m_pNormalImage->setScaleX(-1.0f); // it works!

    backBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"), this, menu_selector(ScriptsLayer::callbackMovePage));
    backBtn->setTag(-1);
    backBtn->setID("arrow-back");


    arrowMenu->addChild(nextBtn);
    arrowMenu->addChild(backBtn);

    float buttonGap = m_scriptsListLayer->getContentWidth() + 50;

    arrowMenu->setLayout(
        SimpleRowLayout::create()
        ->setMainAxisAlignment(MainAxisAlignment::Center)
        ->setMainAxisDirection(AxisDirection::RightToLeft)
        ->setGap(buttonGap)
    );

    this->addChild(arrowMenu);

    this->loadPage(1); // load the first page after all nodes are added as if we call it too early backBtn and nextBtn would be nullptr
}

void ScriptsLayer::callbackMovePage(CCObject* object) {
    this->loadPage(this->currentPage + object->getTag());
}

bool ScriptsLayer::init(bool plugin) {
    if (!CCLayer::init()) return false;
    this->setKeypadEnabled(true);
    winSize = CCDirector::get()->getWinSize();

    this->currentPage = 1;
    this->itemsPerPage = 10;
    this->plugin = plugin;
    this->infoLabel = CCLabelBMFont::create("", "goldFont.fnt");

    this->infoLabel->setScale(0.5f);
    this->infoLabel->setAnchorPoint({1.0f, 1.0f});

    auto background = geode::createLayerBG();
    background->setID("background"); // background is better than bg imo
    background->setZOrder(-1);
    auto array = CCArray::create();

    geode::addSideArt(this);

    auto backMenu = CCMenu::create();
    backMenu->setID("back-menu");
    
    auto backBtn = CCMenuItemExt::createSpriteExtra(CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"), [this](CCObject*) {
        this->keyBackClicked();
    });
    backMenu->addChild(backBtn);

    // some parts of the quit button code is taken from geode's own source code
    // more specifically at
    // https://github.com/geode-sdk/geode/blob/0da07978549a898c98ff54efdeb08dda3dc55787/loader/src/ui/mods/ModsLayer.cpp#L371

    backMenu->setAnchorPoint({0.0f, 0.5f});

    backMenu->setLayout(SimpleRowLayout::create()
        ->setMainAxisAlignment(MainAxisAlignment::Start)
        ->setGap(5.0f)
    );
    this->addChildAtPosition(backMenu, Anchor::TopLeft, {12.0f, -25.0f}, false);

    this->addChild(background);

    auto actionsMenu = CCMenu::create();
    actionsMenu->setLayout(SimpleColumnLayout::create()
        ->setMainAxisAlignment(MainAxisAlignment::Start)
        ->setGap(0.5f)
        ->setMainAxisDirection(AxisDirection::BottomToTop)
    );
    actionsMenu->setAnchorPoint({0.0f, 0.0f});
    actionsMenu->setContentSize({38.0f, 200.0f});
    actionsMenu->setID("actions-menu");


    auto pluginsBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create(this->plugin ? "script.png"_spr : "plugin.png"_spr), CircleBaseColor::Green, CircleBaseSize::Small), [this](CCObject*) {
        auto layer = ScriptsLayer::scene(!this->plugin);
        CCDirector::get()->replaceScene(CCTransitionFade::create(0.5f, layer)); // we shouldnt really do pushScene here, as then pressing the button repeatedly will cause the scene stack to fill up
        // doing popScene after replaceScene was called will send you back to the main menu.
    });
    pluginsBtn->setID("plugins-btn");

    auto JeomETRYdASH = CCMenuItemSpriteExtra::create(CircleButtonSprite::create(CCSprite::create(this->plugin ? "plugin_import.png"_spr : "script_import.png"_spr), CircleBaseColor::Green, CircleBaseSize::Small), this, menu_selector(ScriptsLayer::importPlugin));
    JeomETRYdASH->setID("import-btn");

    actionsMenu->addChild(pluginsBtn);
    actionsMenu->addChild(JeomETRYdASH);

    auto rightActionsMenu = CCMenu::create();
    rightActionsMenu->setLayout(SimpleColumnLayout::create()
        ->setMainAxisAlignment(MainAxisAlignment::Start)
        ->setGap(0.5f)
        ->setMainAxisDirection(AxisDirection::BottomToTop)
    );
    rightActionsMenu->setAnchorPoint({1.0f, 0.0f});
    rightActionsMenu->setContentSize({38.0f, 200.0f});
    rightActionsMenu->setID("right-actions-menu");

    auto restartSpr = CCSprite::createWithSpriteFrameName("geode.loader/reload.png");
    restartSpr->setColor({255, 215, 65});
    auto restartBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(restartSpr, CircleBaseColor::Green, CircleBaseSize::Small), [](CCObject* sender) {
        geode::createQuickPopup(
            "Restart Game",
            "Would you like to restart?",
            "Cancel", "Restart",
            [](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    game::restart(true);
                }
            }
        );
    });

    restartBtn->setID("restart-btn");

    auto sextingsBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::createWithSpriteFrameName("geode.loader/settings.png", 1.0f, CircleBaseColor::Green, CircleBaseSize::Small), [](CCObject*) {
        openSettingsPopup(Mod::get(), true);
    });
    sextingsBtn->setID("settings-btn");

    rightActionsMenu->addChild(sextingsBtn);
    rightActionsMenu->addChild(restartBtn);

    this->addChildAtPosition(actionsMenu, Anchor::BottomLeft, {5.0f, 0.0f}, false);
    this->addChildAtPosition(rightActionsMenu, Anchor::BottomRight, {-5.0f, 0.0f}, false);

    this->setupScriptsList();

    this->addChildAtPosition(infoLabel, Anchor::TopRight, {-3.5f, -0.5f}, false);

    actionsMenu->updateLayout();
    rightActionsMenu->updateLayout();
    backMenu->updateLayout();

    return true;
}

void ScriptsLayer::startImport(std::filesystem::path path, std::filesystem::path dest, std::filesystem::copy_options options) {
	std::error_code ec;
    std::filesystem::copy_file(path, dest, options, ec);

    if (ec) {
        FLAlertLayer::create(
            "Error",
            fmt::format("An error occurred importing {}!\nErr: {}", path.filename(), ec.message()),
            "OK"
        )->show();
    } else {
        if (this->plugin) Mod::get()->setSavedValue<bool>(fmt::format("safe-{}", path.stem()), true);
		else if (!Mod::get()->hasSavedValue(fmt::format("enabled-{}", path.stem()))) Mod::get()->setSavedValue<bool>(fmt::format("enabled-{}", path.stem()), true); // auto enable script on import
        geode::createQuickPopup(
            "Success",
			fmt::format("{} has been imported successfully!\nWould you like to restart for it to take effect?", path.filename()),
            "Cancel", "Restart",
            [](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    utils::game::restart(true);
                }
            }
        );
    }
}

void ScriptsLayer::importPlugin(CCObject*) {
    geode::createQuickPopup(
        "Import",
        fmt::format("Would you like to import a {}?", this->plugin ? "plugin" : "script"),
        "Cancel", "Import",
        [this](FLAlertLayer*, bool btn2) {
            if (btn2) {
                async::spawn(file::pick(file::PickMode::OpenFile, file::FilePickOptions {
                    .filters = { file::FilePickOptions::Filter {
                        .description = this->plugin ? "SerpentLua Plugins" : "Lua Scripts",
                        .files = { this->plugin ? "*.slp" : "*.lua"}
                    }}
                }), ([this](geode::Result<std::optional<std::filesystem::path>> result) {
                    if (result.isErr()) {
                        FLAlertLayer::create(
                            "Error",
                            fmt::format("An error occurred importing: \n", *(result.err())).c_str(),
                            "OK"
                        )->show();
                        return;
                    }

                    auto opt = result.unwrap();

                    if (!opt) return; // cancelled
                    auto path = *opt;
                    geode::createQuickPopup(
                        "Confirm",
                        fmt::format("Are you sure you want to import {}?", path.filename()),
                        "Cancel", "Confirm",
                        [=, this](FLAlertLayer*, bool btn2) {
                            if (btn2) {
                                auto dest = Mod::get()->getConfigDir()/(this->plugin ? "plugins" : "scripts")/path.filename();

                                std::error_code ec;
								if (Mod::get()->getSettingValue<bool>("overwrite")) {
									this->startImport(path, dest, std::filesystem::copy_options::overwrite_existing);
									return;
								}
                                if (std::filesystem::exists(dest)) {
									std::string tip = !Mod::get()->setSavedValue<bool>("show-overwrite-tip", true) ? "\n<cy>Tip</c>: Enabling the <cr>\"Skip Overwrite Confirmation\"</c> setting will skip overwrite confirmation in the future." : "";
                                    geode::createQuickPopup(
                                        "File Exists",
                                        fmt::format("{} already exists! would you like to overwrite it?{}", path.filename(), tip),
                                        "Cancel", "Overwrite",
                                        [path, dest, this](FLAlertLayer*, bool btn2) {
                                            if (!btn2) return;

                                            this->startImport(path, dest, std::filesystem::copy_options::overwrite_existing);
                                        }
                                    );
                                } else {
                                    this->startImport(path, dest, std::filesystem::copy_options::none);
                                }
                            }
                        }
                    );
                }));
            }
        });
}

ScriptsLayer* ScriptsLayer::create(bool plugin) {
    auto ret = new ScriptsLayer();
    if (ret->init(plugin)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCScene* ScriptsLayer::scene(bool plugin) {
    auto scene = CCScene::create();
    scene->addChild(ScriptsLayer::create(plugin));
    return scene;
}

void ScriptsLayer::keyBackClicked() {
    CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}