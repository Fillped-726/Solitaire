#include "GameController.h"
#include "services/GameRuleService.h"
#include "commands/MoveCardCommand.h"
#include "configs/CardResConfig.h"
#include "configs/LevelConfigLoader.h"
#include "services/GameModelGenerator.h"
#include "commands/DrawCardCommand.h"
#include "views/LevelSelectScene.h"

USING_NS_CC;

Scene* GameController::createGameScene(int levelId) {
    auto scene = Scene::create();

    // 1. 创建 View
    auto view = GameView::create();
    scene->addChild(view);

    // 2. 创建 Model
    // (根据之前的代码，GameModel 是一个普通 C++ 类，或者继承自 Ref)
    // 如果 GameModel 是普通类：
    auto model = new GameModel();
    // 如果 GameModel 也是 Ref 且有 create()，则用 auto model = GameModel::create();

    // 3. [修正] 使用 create 工厂方法创建 Controller
    // 必须传入 view 和 model 两个参数
    auto controller = GameController::create(view, model);

    if (controller) {
        // 4. 设置关卡 ID (createGameScene 是静态成员，可以访问私有变量)
        controller->_currentLevelId = levelId;

        // 5. 启动游戏
        controller->startGame();

        // 6. [关键] 保持生命周期
        // Controller 是 autorelease 的。如果没人持有它，这一帧结束它就会被释放。
        // 我们把 Controller 绑定到 View 的 UserObject 上，让 View 拿着它。
        view->setUserObject(controller);
    }

    return scene;
}

GameController* GameController::create(GameView* view, GameModel* model) {
    GameController* ret = new (std::nothrow) GameController();
    if (ret && ret->init(view, model)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameController::init(GameView* view, GameModel* model) {
    _gameView = view;
    _gameModel = model;

    // 保持引用，防止被销毁
    _gameView->retain();
    _gameModel->retain();


    _undoManager = std::make_unique<UndoManager>();

    return true;
}

void GameController::startGame() {
    CCLOG(">>> GameController: Start Game Flow");

    // 1. 加载配置
    std::string filename = StringUtils::format("level_%d.json", _currentLevelId);
    CCLOG(">>> Loading Level: %s", filename.c_str());

    LevelConfig config = LevelConfigLoader::loadLevel(filename);
    if (!config.isValid()) {
        CCLOG("!!! Error: Level config is invalid. Check Resources/%s", filename.c_str());
        return;
    }

    // 2. 生成数据 (Generator 会根据 JSON 把底牌设为 Discard，备用牌设为 Deck)
    GameModelGenerator::generate(_gameModel, config);

    // 3. 计算遮挡关系
    GameRuleService::updateFaceUpStates(_gameModel);

    // 4. 清理旧视图
    _gameView->clearBoard();
    _undoManager->clear(); // [移到这里] 新开局必须清空历史

    // 5. 创建卡牌视图
    auto& allCards = _gameModel->getAllCards();

    for (auto card : allCards) {
        // 跳过被 JSON 剔除的废牌
        if (card->getState() == CardState::Removed) continue;

        // A. 主牌区 (Playfield)
        if (card->getState() == CardState::Playfield) {
            auto cardView = CardView::create(card);
            cardView->setOnClickCallback([this](int id) { this->handleCardClick(id); });
            _gameView->addCardToLayer(cardView, card->getPosition(), card->getZOrder());
        }
        // B. 底牌区 (Discard) - 这里的逻辑由 Generator 决定
        else if (card->getState() == CardState::Discard) {
            auto cardView = CardView::create(card);
            // 底牌不需要点击回调，只需要显示在固定位置
            _gameView->addCardToLayer(cardView, card->getPosition(), 0);
        }
        // C. 备用牌堆 (Deck) - 隐藏
        else if (card->getState() == CardState::Deck) {
            auto cardView = CardView::create(card);
            cardView->setVisible(false); // 隐藏
            cardView->setPosition(Vec2(200, 300));
            // 绑定点击回调 (虽然在堆里点不到，但翻出来后需要能点)
            cardView->setOnClickCallback([this](int id) { this->handleCardClick(id); });
            _gameView->addCardToLayer(cardView, Vec2(200, 300), 0);
        }
    }

    // ============================================================
    // 6. [关键修复] 创建 UI 控件 (移出 if baseCard 块)
    // ============================================================

    // --- 创建 UNDO 按钮 ---
    auto undoLabel = Label::createWithSystemFont("UNDO", "Arial", 60);
    undoLabel->enableOutline(Color4B::BLACK, 2);

    auto undoItem = MenuItemLabel::create(undoLabel, [this](Ref* sender) {
        this->handleUndo();
        });
    undoItem->setPosition(Vec2(900, 200));

    auto menu = Menu::create(undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    _gameView->addChild(menu, 1000); // 确保在最上层

    // --- 创建 牌堆点击区域 (Draw Pile) ---
    std::string backPath = CardResConfig::getBackImagePath();
    auto drawPile = Sprite::create(backPath);

    if (drawPile) {
        drawPile->setPosition(Vec2(200, 300));
        if (drawPile->getContentSize().width > 0) {
            drawPile->setScale(180.0f / drawPile->getContentSize().width);
        }
        _gameView->addChild(drawPile, -1); // 放在最底层作为背景

        // 绑定点击事件
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [drawPile](Touch* t, Event* e) {
            // 只响应点击图片范围内的操作
            auto target = static_cast<Sprite*>(e->getCurrentTarget());
            Vec2 p = target->convertToNodeSpace(t->getLocation());
            Rect rect = Rect(0, 0, target->getContentSize().width, target->getContentSize().height);
            return rect.containsPoint(p);
            };
        listener->onTouchEnded = [this](Touch* t, Event* e) {
            this->handleDrawPileClick();
            };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, drawPile);
    }
}

void GameController::handleCardClick(int cardId) {
    CCLOG(">>> Controller received click: %d", cardId);

    // 1. 获取数据
    auto clickedCard = _gameModel->getCardById(cardId);
    int currentBaseId = _gameModel->getTopCardId();
    auto baseCard = _gameModel->getCardById(currentBaseId);

    if (!clickedCard || !baseCard) return;

    // 如果牌是背面朝上，禁止操作！
    if (!clickedCard->isFaceUp()) {
        CCLOG(">>> Card %d is blocked (Face Down).", cardId);
        return;
    }

    // 2. 规则判定 
    if (GameRuleService::canMatch(clickedCard, baseCard)) {
        CCLOG(">>> MATCH SUCCESS! Creating Command...");

        Vec2 fromPos = clickedCard->getPosition();
        Vec2 toPos = Vec2(540, 300);

        // [关键修复] 
        // 第4个参数 (oldTopCardId) 必须直接传 baseCard->getId()
        // 绝对不能传 _topDiscardCardId (那个变量已经删了)
        auto cmd = std::make_unique<MoveCardCommand>(
            _gameModel, _gameView,
            cardId,
            baseCard->getId(), // <--- 这里！这里！一定要用 baseCard->getId()
            fromPos, toPos
        );

        _undoManager->addCommand(std::move(cmd));

        // 消除了一张牌，可能解锁了上面的牌
        this->refreshBoardState();

        this->checkGameState();

    }
    else {
        CCLOG(">>> MATCH FAILED! Rules: |%d - %d| != 1",
            (int)clickedCard->getFace(), (int)baseCard->getFace());

        // 视觉反馈：摇晃一下表示不行 (可选)
    }
}

void GameController::handleUndo() {
    CCLOG(">>> Undo Button Clicked");
    if (_undoManager) {
        _undoManager->undo();

        // 牌飞回来了，重新计算遮挡
        this->refreshBoardState();
    }

}

void GameController::handleDrawPileClick() {
    CCLOG(">>> Draw Pile Clicked");

    // [修改] 直接问 Model 下一张牌是谁
    int nextCardId = _gameModel->popNextDrawCard();

    if (nextCardId == -1) {
        CCLOG(">>> No more cards in deck!");
        return;
    }

    // 创建命令
    int currentTopId = _gameModel->getTopCardId();
    auto cmd = std::make_unique<DrawCardCommand>(
        _gameModel, _gameView, 
        nextCardId, currentTopId
    );

    _undoManager->addCommand(std::move(cmd));
    
    this->checkGameState();
}

void GameController::checkGameState() {
    // 1. 胜利检查：桌面没牌了
    int remaining = _gameModel->getPlayfieldCardCount();
    if (remaining == 0) {
        CCLOG(">>> WINNER!");
        showGameOverUI(true);
        return; // 赢了就不用检查输了
    }

    // 2. [新增] 失败检查
    // 条件 A: 备用牌堆(Deck)没牌了
    int deckCount = 0;
    for (auto card : _gameModel->getAllCards()) {
        if (card->getState() == CardState::Deck) {
            deckCount++;
        }
    }

    // 条件 B: 且桌面上没有任何牌能跟底牌匹配
    if (deckCount == 0 && !GameRuleService::hasAnyMove(_gameModel)) {
        CCLOG(">>> GAME OVER! No moves left.");
        showGameOverUI(false); // 传入 false 显示失败界面
    }
}

void GameController::showGameOverUI(bool win) {
    // 1. 灰半透明背景 (遮罩)
    auto mask = LayerColor::create(Color4B(0, 0, 0, 150));
    _gameView->addChild(mask, 2000); // Z=2000 保证在最上层

    // 屏蔽下层点击
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, mask);

    // 2. 显示文字
    std::string titleStr = win ? "YOU WIN!" : "GAME OVER";
    auto label = Label::createWithSystemFont(titleStr, "Arial", 100);
    label->setPosition(Vec2(540, 1200));
    label->setColor(win ? Color3B::YELLOW : Color3B::GRAY);
    mask->addChild(label);

    auto replayLabel = Label::createWithSystemFont("REPLAY", "Arial", 50);
    auto replayItem = MenuItemLabel::create(replayLabel, [this](Ref*) {
        // 重玩当前关卡
        Director::getInstance()->replaceScene(GameController::createGameScene(_currentLevelId));
        });
    replayItem->setPosition(Vec2(340, 600));

    // 按钮 2: 返回选关菜单
    auto menuLabel = Label::createWithSystemFont("MENU", "Arial", 50);
    auto menuItem = MenuItemLabel::create(menuLabel, [](Ref*) {
        // 还没创建这个场景，下一节我们写
        // 暂时先 popScene 或者去 LevelSelectScene
        Director::getInstance()->replaceScene(LevelSelectScene::createScene());
        });
    menuItem->setPosition(Vec2(740, 600));

    auto menu = Menu::create(replayItem, menuItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    mask->addChild(menu);
}

void GameController::refreshBoardState() {
    // 1. 纯数据层：计算谁该翻面
    GameRuleService::updateFaceUpStates(_gameModel);

    // 2. 视图层：让 View 根据新的 Model 状态更新贴图 (变正面或背面)
    auto& allCards = _gameModel->getAllCards();
    for (auto card : allCards) {
        if (card->getState() == CardState::Playfield) {
            auto cardView = _gameView->getCardViewById(card->getId());
            if (cardView) {
                // 这一步会调用 CardView::updateView()，自动切换 card_xx.png 或 card_back.png
                cardView->updateView();
            }
        }
    }
}