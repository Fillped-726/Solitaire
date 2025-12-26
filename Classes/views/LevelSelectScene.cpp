#include "LevelSelectScene.h"
#include "controllers/GameController.h"

USING_NS_CC;

Scene* LevelSelectScene::createScene() {
    return LevelSelectScene::create();
}

bool LevelSelectScene::init() {
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 背景
    auto bg = LayerColor::create(Color4B(50, 50, 80, 255));
    this->addChild(bg);

    // 2. 标题
    auto label = Label::createWithSystemFont("SELECT LEVEL", "Arial", 80);
    label->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 200));
    this->addChild(label);

    // 3. 关卡按钮生成
    Vector<MenuItem*> menuItems;
    for (int i = 1; i <= 3; ++i) {
        std::string text = StringUtils::format("Level %d", i);
        auto itemLabel = Label::createWithSystemFont(text, "Arial", 60);

        auto item = MenuItemLabel::create(itemLabel, [i](Ref*) {
            // 切换场景
            auto gameScene = GameController::createGameScene(i);
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, gameScene));
            });

        item->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 400 - (i * 150)));
        menuItems.pushBack(item);
    }

    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);

    return true;
}