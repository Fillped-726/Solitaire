#include "GameController.h"
#include "services/GameRuleService.h"
#include "commands/MoveCardCommand.h"
#include "configs/CardResConfig.h"
#include "configs/LevelConfigLoader.h"
#include "services/GameModelGenerator.h"
#include "commands/DrawCardCommand.h"
#include "views/LevelSelectScene.h"
#include "views/CardView.h" // 确保包含 CardView 头文件

USING_NS_CC;

// 析构函数：必须释放 retain 的对象
GameController::~GameController() {
    CC_SAFE_RELEASE(_gameView);
    CC_SAFE_RELEASE(_gameModel);
}

Scene* GameController::createGameScene(int levelId) {
    auto scene = Scene::create();
    auto view = GameView::create();
    scene->addChild(view);

    auto model = GameModel::create(); // 假设 GameModel 也是 Ref
    auto controller = GameController::create(view, model);

    if (controller) {
        controller->_currentLevelId = levelId;
        controller->startGame();
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

    // 手动管理引用计数 (配对析构函数中的 release)
    _gameView->retain();
    _gameModel->retain();

    _undoManager = std::make_unique<UndoManager>();
    return true;
}

// --- 核心流程：被拆分成了三个清晰的步骤 ---

void GameController::startGame() {
    CCLOG(">>> GameController: Start Game Flow");

    // 1. 加载数据
    if (!_loadLevel()) {
        return; // 加载失败
    }

    // 2. 清理环境
    _gameView->clearBoard();
    _undoManager->clear();

    // 3. 计算初始遮挡关系
    GameRuleService::updateFaceUpStates(_gameModel);

    // 4. 创建视图
    _initCardViews();
    _initSceneUI();
}

bool GameController::_loadLevel() {
    std::string filename = StringUtils::format("level_%d.json", _currentLevelId);
    LevelConfig config = LevelConfigLoader::loadLevel(filename);

    if (!config.isValid()) {
        CCLOG("!!! Error: Level config is invalid: %s", filename.c_str());
        return false;
    }

    // Generator 负责将 Config 转化为 Model 数据
    // 并将备用牌 ID 填入 drawStackIds (前提是您修复了 Generator)
    GameModelGenerator::generate(_gameModel, config);
    return true;
}

void GameController::_initCardViews() {
    auto& allCards = _gameModel->getAllCards();

    for (auto card : allCards) {
        if (card->getState() == CardState::Removed) continue;

        // 创建通用 CardView
        auto cardView = CardView::create(card);

        // 绑定通用点击事件
        cardView->setOnClickCallback([this](int id) {
            this->handleCardClick(id);
            });

        // 根据状态设置位置和可见性
        if (card->getState() == CardState::Playfield) {
            _gameView->addCardToLayer(cardView, card->getPosition(), card->getZOrder());
        }
        else if (card->getState() == CardState::Discard) {
            // 底牌暂时不需要点击，但显示出来
            _gameView->addCardToLayer(cardView, card->getPosition(), 0);
        }
        else if (card->getState() == CardState::Deck) {
            // 备用牌堆：隐藏在固定位置
            cardView->setVisible(false);
            // 放到 View 指定的堆叠位置 (建议 View 提供接口，这里暂时硬编码)
            cardView->setPosition(Vec2(200, 300));
            _gameView->addCardToLayer(cardView, Vec2(200, 300), 0);
        }
    }
}

void GameController::_initSceneUI() {
    // 1. 创建 UNDO 按钮
    // 建议：View 应该提供 addUndoButton 接口，这里暂时保留实现
    auto undoLabel = Label::createWithSystemFont("UNDO", "Arial", 60);
    undoLabel->enableOutline(Color4B::BLACK, 2);
    auto undoItem = MenuItemLabel::create(undoLabel, [this](Ref*) { this->handleUndo(); });
    undoItem->setPosition(Vec2(900, 200));

    auto menu = Menu::create(undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    _gameView->addChild(menu, 1000);

    // 2. 创建 牌堆点击区域 (Draw Pile)
    // 这是一个纯逻辑的触摸区，或者一个永远在背面的 Sprite
    std::string backPath = CardResConfig::getBackImagePath();
    auto drawPile = Sprite::create(backPath);
    if (drawPile) {
        drawPile->setPosition(Vec2(200, 300));
        // 缩放逻辑
        if (drawPile->getContentSize().width > 0) {
            drawPile->setScale(180.0f / drawPile->getContentSize().width);
        }
        _gameView->addChild(drawPile, -1);

        // 绑定监听器
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [drawPile](Touch* t, Event* e) {
            Vec2 p = drawPile->convertToNodeSpace(t->getLocation());
            Rect rect = Rect(0, 0, drawPile->getContentSize().width, drawPile->getContentSize().height);
            return rect.containsPoint(p);
            };
        listener->onTouchEnded = [this](Touch* t, Event* e) {
            this->handleDrawPileClick();
            };
        Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, drawPile);
    }
}

// --- 业务逻辑 ---

void GameController::handleCardClick(int cardId) {
    auto clickedCard = _gameModel->getCardById(cardId);
    int currentBaseId = _gameModel->getTopCardId();
    auto baseCard = _gameModel->getCardById(currentBaseId);

    // 防御性检查
    if (!clickedCard || !baseCard) return;
    if (!clickedCard->isFaceUp()) return; // 背面牌不可点

    // 规则判定
    if (GameRuleService::canMatch(clickedCard, baseCard)) {
        // 创建移动命令
        Vec2 fromPos = clickedCard->getPosition();
        Vec2 toPos = Vec2(540, 300); // 建议从 View 获取底牌区位置

        // 关键：OldTopId 必须是 baseCard 的 ID
        auto cmd = std::make_unique<MoveCardCommand>(
            _gameModel, _gameView,
            cardId, baseCard->getId(),
            fromPos, toPos
        );
        _undoManager->addCommand(std::move(cmd));

        refreshBoardState();
        checkGameState();
    }
    else {
        // 可选：播放 Shake 动画
        CCLOG(">>> No Match!");
    }
}

void GameController::handleDrawPileClick() {
    int nextCardId = _gameModel->popNextDrawCard();
    if (nextCardId == -1) {
        CCLOG(">>> Deck is empty");
        return;
    }

    int currentTopId = _gameModel->getTopCardId();
    auto cmd = std::make_unique<DrawCardCommand>(
        _gameModel, _gameView,
        nextCardId, currentTopId
    );
    _undoManager->addCommand(std::move(cmd));

    // 发牌可能导致游戏失败（如果没有牌了且没步数了）
    checkGameState();
}

void GameController::handleUndo() {
    if (_undoManager) {
        _undoManager->undo();
        refreshBoardState();
    }
}

void GameController::refreshBoardState() {
    // 1. 逻辑计算
    GameRuleService::updateFaceUpStates(_gameModel);

    // 2. 视图更新
    auto& allCards = _gameModel->getAllCards();
    for (auto card : allCards) {
        if (card->getState() == CardState::Playfield) {
            auto cardView = _gameView->getCardViewById(card->getId());
            if (cardView) cardView->updateView();
        }
    }
}

void GameController::checkGameState() {
    // 1. 胜利判定
    if (_gameModel->getPlayfieldCardCount() == 0) {
        showGameOverUI(true);
        return;
    }

    // 2. 失败判定 (备用牌空 且 无路可走)
    // 优化：不直接遍历 View，而是信任 Model 数据
    if (_gameModel->getDrawStackSize() == 0 && !GameRuleService::hasAnyMove(_gameModel)) {
        showGameOverUI(false);
    }
}

void GameController::showGameOverUI(bool win) {
    // 这是一个 View 层的逻辑，但暂时放在这里，通过辅助代码组织
    auto mask = LayerColor::create(Color4B(0, 0, 0, 150));
    _gameView->addChild(mask, 2000);

    // 屏蔽点击
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch*, Event*) { return true; };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, mask);

    // 标题
    auto label = Label::createWithSystemFont(win ? "YOU WIN!" : "GAME OVER", "Arial", 100);
    label->setPosition(Vec2(540, 1200)); // 建议使用 _gameView->getContentSize().width / 2
    label->setColor(win ? Color3B::YELLOW : Color3B::GRAY);
    mask->addChild(label);

    // 菜单
    auto replayItem = MenuItemLabel::create(
        Label::createWithSystemFont("REPLAY", "Arial", 50),
        [this](Ref*) {
            Director::getInstance()->replaceScene(GameController::createGameScene(_currentLevelId));
        }
    );
    replayItem->setPosition(Vec2(340, 600));

    auto menuItem = MenuItemLabel::create(
        Label::createWithSystemFont("MENU", "Arial", 50),
        [](Ref*) {
            Director::getInstance()->replaceScene(LevelSelectScene::createScene());
        }
    );
    menuItem->setPosition(Vec2(740, 600));

    auto menu = Menu::create(replayItem, menuItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    mask->addChild(menu);
}