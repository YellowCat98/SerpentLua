#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

void ScriptsLayer::refreshWith(CCArray* array) {
    this->m_scriptsListLayer->m_listView->removeFromParent();

    this->m_scriptsListLayer->m_listView = ListView::create(array, 30.0f);
    m_scriptsListLayer->addChild(m_scriptsListLayer->m_listView);
}

void ScriptsLayer::loadPage(int page) {
    //auto allScripts = internal::RuntimeManager::get()->getAllScripts();

    std::vector<ScriptMetadata*> scriptsInPage;

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
        array->addObject(ScriptItem::create(item, [](CCMenuItemToggler*){}, CCSize(358.0f, 30)));
    }

    //backBtn->setVisible(true);
    //nextBtn->setVisible(true);

    if (scriptsInPage.front() == scripts.begin()->second) { // basically if were on the first page
        backBtn->setVisible(false);
    }

    if (scriptsInPage.back() == scripts.rbegin()->second) { // basically if were on the last page
        nextBtn->setVisible(false);
    }

    this->refreshWith(array);

    this->currentPage = page;

}


void ScriptsLayer::setupScriptsList() {
    CCArray* scriptItems = CCArray::create();
    scripts = SerpentLua::internal::RuntimeManager::get()->getAllScripts();
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
    
    m_scriptsListLayer = GJListLayer::create(listView, "scripts", {194, 114, 62, 255}, 358.0f, 220.0f, 0);

    auto listlayerSize = m_scriptsListLayer->getContentSize();
    m_scriptsListLayer->setPosition(ccp((winSize.width - listlayerSize.width)/2, (winSize.height - listlayerSize.height)/2));

    this->addChild(m_scriptsListLayer);

    

    auto arrowMenu = CCMenu::create(); // luckily, positioning a ccmenu at 0, 0 makes it fill the entire screen! meaning i can just do some layout stuff to get these working!

    nextBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"), this, menu_selector(ScriptsLayer::callbackMovePage));
    nextBtn->setTag(1);
    nextBtn->m_pNormalImage->setScaleX(-1.0f); // it works!

    backBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"), this, menu_selector(ScriptsLayer::callbackMovePage));
    backBtn->setTag(-1);


    arrowMenu->addChild(nextBtn);
    arrowMenu->addChild(backBtn);

    auto pluginsBtn = CCMenuItemExt::createSpriteExtra(CircleButtonSprite::create(CCSprite::create("plugin.png"_spr)), [this](CCObject*) {
        this->loadPage(this->currentPage + 1);
    });

    this->getChildByID("actions-menu")->addChild(pluginsBtn);

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

bool ScriptsLayer::init() {
    if (!CCLayer::init()) return false;
    this->setKeypadEnabled(true);
    winSize = CCDirector::get()->getWinSize();

    this->currentPage = 1;
    this->itemsPerPage = 10;

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

    actionsMenu->addChild(restartBtn);
    
    this->addChildAtPosition(actionsMenu, Anchor::BottomLeft, {5.0f, 0.0f}, false);

    this->setupScriptsList();

    actionsMenu->updateLayout();
    backMenu->updateLayout();

    return true;
}


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