#include <internal/ui/ScriptsLayer.hpp>
#include <internal/ui/ScriptItem.hpp>

using namespace SerpentLua::internal::ui;
using namespace geode::prelude;

bool ScriptsLayer::setup() {
    scroll = ScrollLayer::create(CCSize(winSize.width - 125.0f, 230));
    this->setTitle(fmt::format("SerpentLua {}", Mod::get()->getVersion().toVString()).c_str(), "goldFont.fnt");


    /*
    CCPoint versionLabelPos = {m_title->getPositionX() + (m_title->getContentSize().width - 25), m_title->getPositionY()};

    auto versionLabel = CCLabelBMFont::create(Mod::get()->getVersion().toVString().c_str(), "bigFont.fnt");
    versionLabel->setColor({0,255,255});
    versionLabel->setPosition(versionLabelPos);

    versionLabel->setScale(m_title->getScale()/2);

    m_mainLayer->addChild(versionLabel);
*/
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

bool ScriptsLayer::initAnchored() {
    winSize = CCDirector::get()->getWinSize();
    return Popup::initAnchored(winSize.width - 100.0f, 280.0f);
}

ScriptsLayer* ScriptsLayer::create() {
    auto ret = new ScriptsLayer();
    if (ret->initAnchored()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}