#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool ScriptsLayer::init() {
    if (!CCLayer::init()) return false;
    this->setKeypadEnabled(true);
    auto winSize = CCDirector::get()->getWinSize();

    auto background = geode::createLayerBG();
    background->setID("background"); // background is better than bg imo
    background->setZOrder(-1);
    auto array = CCArray::create();

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

    for (auto& script : SerpentLua::internal::RuntimeManager::get()->getAllScripts()) {
        array->addObject(ScriptItem::create(script.second, [](CCMenuItemToggler* button) {
            auto item = typeinfo_cast<ScriptItem*>(button->getParent()->getParent()); // normally this should never return nullptr, but just in case!
            if (!item) return;
            auto metadata = item->metadata;
            Mod::get()->setSavedValue<bool>(fmt::format("enabled-{}", metadata->id), !button->isToggled());
        }, CCSize(358.0f, 30)));
    }

    auto listview = ListView::create(array, 30);
    auto listlayer = GJListLayer::create(listview, "scripts", {194, 114, 62, 255}, 358.0f, 220.0f, 0);

    auto listlayerSize = listlayer->getContentSize();
    listlayer->setPosition(ccp((winSize.width - listlayerSize.width)/2, (winSize.height - listlayerSize.height)/2));

    this->addChild(listlayer);

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

    auto restartBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_updateBtn_001.png", 1.0f, [](CCObject* sender){
        game::restart();
    });

    std::map<std::string, std::string> dummyMetadataMap = {
        {"name", "test"},
        {"developer", "hi"},
        {"version", "1.0.0"},
        {"serpent-version", "yoo"},
        {"plugins", "hi hi hi hi"}
    };

    auto dummyMetadata = ScriptMetadata::create(dummyMetadataMap);
    auto pluginsBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("plugin.png"_spr)), [&](CCObject*) {
        array->addObject(ScriptItem::create(dummyMetadata, [&](CCMenuItemToggler*) {

        }, CCSize(358.0f, 30)));
        //listview->m_tableView->reloadData();
    });

    actionsMenu->addChild(restartBtn);
    actionsMenu->addChild(pluginsBtn);
    
    this->addChildAtPosition(actionsMenu, Anchor::BottomLeft, {5.0f, 0.0f}, false);

    actionsMenu->updateLayout();
    backMenu->updateLayout();

    return true;
}


/*
bool ScriptsLayer::setup() {
    this->setTitle(fmt::format("SerpentLua {}", Mod::get()->getVersion().toVString()));
    CCArray* array = CCArray::create();
    
    /*for (auto& script : SerpentLua::internal::RuntimeManager::get()->getAllScripts()) {
        array->addObject(ScriptItem::create(script.second, [](CCObject*) {

        }, CCSize(winSize.width - 125.0f, 30)));
    }
    for (int i = 0; i < 50; i++) {
        std::map<std::string, std::string> metadata = {
            {"name", fmt::format("test {}", std::to_string(i))},
            {"developer", "hi"},
            {"version", "1.0.0"},
            {"serpent-version", "yoo"},
            {"plugins", "hi hi hi hi"}
        };
        array->addObject(ScriptItem::create(SerpentLua::ScriptMetadata::create(metadata), [](CCObject*) {

        }, CCSize(winSize.width - 125.0f, 30)));
    }
    auto listview = ListView::create(array, 30.0f, winSize.width - 125.0f, 230.0f);
    

    border = Border::create(listview, {100, 45, 0, 255}, {winSize.width - 125.0f, 230});
    border->setPadding(0);

    border->setPosition(
        {
            (m_bgSprite->getPositionX() - (m_bgSprite->getContentWidth() * m_bgSprite->getAnchorPoint().x)) + 10,
            m_bgSprite->getPositionY() - 130
        }
    );

    m_mainLayer->addChild(border);

    listview->m_tableView->updateLayout();
    return true;
}


bool ScriptsLayer::setup() {
    scroll = ScrollLayer::create(CCSize(winSize.width - 125.0f, 230));
    this->setTitle(fmt::format("SerpentLua {}", Mod::get()->getVersion().toVString()).c_str(), "goldFont.fnt");
    
    CCPoint versionLabelPos = {m_title->getPositionX() + (m_title->getContentSize().width - 25), m_title->getPositionY()};

    auto versionLabel = CCLabelBMFont::create(Mod::get()->getVersion().toVString().c_str(), "bigFont.fnt");
    versionLabel->setColor({0,255,255});
    versionLabel->setPosition(versionLabelPos);

    versionLabel->setScale(m_title->getScale()/2);

    m_mainLayer->addChild(versionLabel);

    scroll->setPosition(winSize / 2);
    scroll->m_contentLayer->setLayout(
        ColumnLayout::create()
            ->setAxisReverse(true)
            ->setAxisAlignment(AxisAlignment::End)
            ->setAutoGrowAxis(250.0f)
            ->setGap(5.0f)
    );

    border = Border::create(scroll, {100, 45, 0, 255}, {winSize.width - 125.0f, 230});

    border->setPosition(
        {
            (m_bgSprite->getPositionX() - (m_bgSprite->getContentWidth() * m_bgSprite->getAnchorPoint().x)) + 10,
            m_bgSprite->getPositionY() - 130
        }
    );

    m_mainLayer->addChild(border);

    
    for (auto& script : SerpentLua::internal::RuntimeManager::get()->getAllScripts()) {
        scroll->m_contentLayer->addChild(ScriptItem::create(script.second, [=](CCObject* sender) {
            //if (static_cast<CCMenuItemToggler*>(sender)->isToggled()) {
            //    Mod::get()->setSavedValue<std::string>("enabled-script", script["id"].asString().unwrap());
            //}
            log::info("Your SCRIPT: {}", script.first);
        }, CCSize(scroll->getContentSize().width, 30)));
    }
    

    auto menu = CCMenu::create();

    auto restartBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_updateBtn_001.png", 0.78f, [](CCObject* sender){
        game::restart();
    });
    menu->addChild(restartBtn);
    //auto plusSpr = CircleButtonSprite::createWithSprite("plus_sign.png"_spr);
    //plusSpr->setScale(0.78f);
    //auto addBtn = CCMenuItemSpriteExtra::create(plusSpr, this, menu_selector(ScriptsLayer::onAdd));
    //menu->addChild(addBtn);

    menu->setLayout(RowLayout::create()
        ->setAxis(Axis::Row)
        ->setAxisAlignment(AxisAlignment::Center)
        ->setAutoScale(true)
        ->setGap(5.0f)
        ->setCrossAxisAlignment(AxisAlignment::Center)
    );
    menu->setPosition({70.0f, 258.0f});

    m_buttonMenu->addChild(menu);
    scroll->m_contentLayer->updateLayout();
    scroll->updateLayout();
    scroll->scrollToTop();
    return true;
}
*/

ScriptsLayer* ScriptsLayer::create() {
    auto ret = new ScriptsLayer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCScene* ScriptsLayer::scene() {
    auto scene = CCScene::create();
    scene->addChild(ScriptsLayer::create());
    return scene;
}

void ScriptsLayer::keyBackClicked() {
    CCDirector::get()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}