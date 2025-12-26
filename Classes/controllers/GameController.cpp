#include "GameController.h"
#include "services/GameRuleService.h"
#include "commands/MoveCardCommand.h"
#include "configs/CardResConfig.h"
#include "configs/LevelConfigLoader.h"
#include "services/GameModelGenerator.h"
#include "commands/DrawCardCommand.h"

USING_NS_CC;

Scene* GameController::createGameScene() {
    auto scene = Scene::create();

    // 1. 创建 View
    auto view = GameView::create();
    scene->addChild(view);

    // 2. 创建 Model
    auto model = GameModel::create();

    // 3. 创建 Controller 并绑定
    auto controller = GameController::create(view, model);

    // 将 Controller 挂载到 Scene 的 UserObject 上，防止被释放
    // 这是 Cocos 中保持纯逻辑对象生命周期的常用技巧
    scene->setUserObject(controller);

    // 4. 启动游戏逻辑
    controller->startGame();

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
    CCLOG(">>> GameController: Start Game Flow [Cite: 69]");

    // 1. 加载配置 (Day 1 的成果)
    LevelConfig config = LevelConfigLoader::loadLevel("level_test.json");
    if (!config.isValid()) {
        CCLOG("!!! Error: Level config is invalid. Check Resources/level_test.json");
        return;
    }

    // 2. 生成数据 (Day 5 的成果)
    // 使用 Generator 将配置注入到 Model 中
    GameModelGenerator::generate(_gameModel, config);

    // 3. 渲染视图 (遍历 Model 生成 View)
    // 先清空旧视图
    _gameView->clearBoard(); 

    auto& allCards = _gameModel->getAllCards();

    // 寻找底牌 (我们在 JSON 里没配底牌，这里临时指定最后一张为底牌用于测试)
    // 或者你可以修改 level_test.json 加一个底牌
    // 这里我们先硬编码：找到第一张还在 Deck 里的牌作为底牌
    CardModel* baseCard = nullptr;

    int deckCreatedCount = 0;
    const int MAX_TEST_DECK = 3;

    for (auto card : allCards) {

        // 1. 如果是主牌区 (Playfield) -> 创建并显示
        if (card->getState() == CardState::Playfield) {
            auto cardView = CardView::create(card);
            cardView->setOnClickCallback([this](int id) { this->handleCardClick(id); });
            _gameView->addCardToLayer(cardView, card->getPosition(), card->getZOrder());
        }

        // 2. 如果是备用牌堆 (Deck)
        else if (card->getState() == CardState::Deck) {
            if (baseCard == nullptr) {
                baseCard = card; // 第一张必做底牌
            }
            else {
                // [新增] 测试限制逻辑
                if (deckCreatedCount < MAX_TEST_DECK) {
                    // 正常创建隐藏牌
                    auto cardView = CardView::create(card);
                    cardView->setVisible(false);
                    cardView->setPosition(Vec2(200, 300));
                    cardView->setOnClickCallback([this](int id) { this->handleCardClick(id); });
                    _gameView->addCardToLayer(cardView, Vec2(200, 300), 0);

                    deckCreatedCount++;
                }
                else {
                    card->setState(CardState::Removed);
                }
            }
        }
    }

    // 4. 初始化底牌
    if (baseCard) {
        baseCard->setState(CardState::Discard);
        baseCard->setFaceUp(true);
        baseCard->setPosition(Vec2(540, 300)); // 放在下方固定位置
        _gameModel->setTopCardId(baseCard->getId());

        auto baseView = CardView::create(baseCard);
        _gameView->addCardToLayer(baseView, baseCard->getPosition(), 0);

        CCLOG(">>> Init BaseCard ID: %d", baseCard->getId());

        _undoManager->clear(); // 新开局时，清空历史记录

        auto undoLabel = Label::createWithSystemFont("UNDO", "Arial", 60);
        // 给按钮加个背景色，防止看不清
        undoLabel->enableOutline(Color4B::BLACK, 2);

        auto undoItem = MenuItemLabel::create(undoLabel, [this](Ref* sender) {
            this->handleUndo();
            });
        // 放在右下角，Z轴设高一点防止被卡牌遮挡
        undoItem->setPosition(Vec2(900, 200));

        auto menu = Menu::create(undoItem, nullptr);
        menu->setPosition(Vec2::ZERO);
        _gameView->addChild(menu, 1000); // Z=1000，确保在最上层
    }

    std::string backPath = CardResConfig::getBackImagePath();
    auto drawPile = Sprite::create(backPath);

    // [关键修复] 检查图片是否加载成功
    if (drawPile == nullptr) {
        CCLOG("!!! CRITICAL ERROR: Could not load image at path: %s", backPath.c_str());
        CCLOG("!!! Please check if the file exists in Resources folder.");
        // 为了不崩溃，创建一个红块代替 (临时方案)
        // drawPile = (Sprite*)LayerColor::create(Color4B::RED, 150, 200);
        // 或者直接 return，防止后面崩溃
        return;
    }

    drawPile->setPosition(Vec2(200, 300));
    // [新增] 如果图片太大，记得缩放一下，不然会盖住半个屏幕
    // 假设背面图和正面图一样大，用 180 / 宽度
    if (drawPile->getContentSize().width > 0) {
        drawPile->setScale(180.0f / drawPile->getContentSize().width);
    }

    _gameView->addChild(drawPile, 0);

    // 绑定点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [drawPile](Touch* t, Event* e) {
        return drawPile->getBoundingBox().containsPoint(t->getLocation());
        };
    listener->onTouchEnded = [this](Touch* t, Event* e) {
        this->handleDrawPileClick();
        };
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, drawPile);
}

void GameController::handleCardClick(int cardId) {
    CCLOG(">>> Controller received click: %d", cardId);

    // 1. 获取数据
    auto clickedCard = _gameModel->getCardById(cardId);
    int currentBaseId = _gameModel->getTopCardId();
    auto baseCard = _gameModel->getCardById(currentBaseId);

    if (!clickedCard || !baseCard) return;

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
    _undoManager->undo();

    // 实际项目中，Undo 后可能需要更新 _topDiscardCardId
    // 但这里为了简化，我们先看动画效果
}

void GameController::handleDrawPileClick() {
    CCLOG(">>> Draw Pile Clicked");

    // 1. 找一张还在 Deck 里的牌
    // 简单逻辑：找列表里第一个状态是 Deck 的
    CardModel* nextCard = nullptr;
    for (auto card : _gameModel->getAllCards()) {
        if (card->getState() == CardState::Deck) {
            nextCard = card;
            break;
        }
    }

    if (!nextCard) {
        CCLOG(">>> No more cards in deck!");
        return;
    }

    // 2. 创建命令
    int currentTopId = _gameModel->getTopCardId();
    auto cmd = std::make_unique<DrawCardCommand>(
        _gameModel, _gameView,
        nextCard->getId(), currentTopId
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

    // 3. 重玩按钮
    auto restartLabel = Label::createWithSystemFont("RESTART", "Arial", 60);
    auto restartItem = MenuItemLabel::create(restartLabel, [this](Ref* sender) {
        // 简单粗暴：重启整个 Scene
        Director::getInstance()->replaceScene(GameController::createGameScene());
        });
    restartItem->setPosition(Vec2(540, 800));

    auto menu = Menu::create(restartItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    mask->addChild(menu);
}