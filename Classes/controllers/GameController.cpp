#include "GameController.h"
#include "services/GameRuleService.h"
#include "commands/MoveCardCommand.h"

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
    CCLOG(">>> GameController: Game Started!");

    // 1. 初始化数据
    _gameModel->initStandardDeck();

    // 2. 模拟发牌逻辑（Day 4 简化版）
    // 我们手动挑两张牌出来测试：一张在底牌区，一张在桌面
    auto& cards = _gameModel->getAllCards();

    // 假设第0张是底牌 (Discard Pile)
    auto baseCard = cards.at(0); // 比如是 黑桃A (Face 1)
    baseCard->setFaceUp(true);
    baseCard->setState(CardState::Discard);

    _gameModel->setTopCardId(baseCard->getId());
    _topDiscardCardId = baseCard->getId();

    // 假设第1张是桌面牌 (Playfield) -> 找一张能匹配的 (Face 2)
    // 为了测试成功，我们强制修改第1张牌的数据
    auto playCard = cards.at(1);
    // 强制改为 黑桃2，确保能匹配上
    // 注意：这里是黑客写法，正常应该去搜寻
    // 但我们的 create(id, face, suit) 并没有 setFace 方法，所以这里先搜一个 Face 2 的出来
    for (auto c : cards) {
        if (c->getFace() == CardFace::Two) {
            playCard = c;
            break;
        }
    }
    playCard->setFaceUp(true); // 正面
    playCard->setState(CardState::Playfield);

    // 3. 创建 View 节点
    // 创建底牌 View
    auto baseView = CardView::create(baseCard);
    baseView->setPosition(Vec2(540, 300)); // 放在下方
    _gameView->addCardToLayer(baseView, Vec2(540, 300), 0);

    // 创建桌面牌 View
    auto playView = CardView::create(playCard);
    // [关键] 绑定点击事件到 Controller
    playView->setOnClickCallback([this](int id) {
        this->handleCardClick(id);
        });
    _gameView->addCardToLayer(playView, Vec2(540, 1200), 0); // 放在上方

    // 创建一个简单的文本按钮
        auto undoLabel = Label::createWithSystemFont("UNDO", "Arial", 60);
    auto undoItem = MenuItemLabel::create(undoLabel, [this](Ref* sender) {
        this->handleUndo();
        });
    undoItem->setPosition(Vec2(900, 200)); // 放在右下角

    auto menu = Menu::create(undoItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    _gameView->addChild(menu, 100);

    CCLOG(">>> Setup Complete: BaseCard(ID:%d, Face:%d), PlayCard(ID:%d, Face:%d)",
        baseCard->getId(), (int)baseCard->getFace(),
        playCard->getId(), (int)playCard->getFace());
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

        // 获取当前 View 的位置 (为了知道Undo飞回哪里)
        // 简单起见，我们假设上面的牌固定在 (540, 1200)，下面的在 (540, 300)
        // 实际项目应该用 _gameView->getCardViewById(cardId)->getPosition();
        Vec2 fromPos = Vec2(540, 1200);
        Vec2 toPos = Vec2(540, 300);

        // 创建命令对象
        auto cmd = std::make_unique<MoveCardCommand>(
            _gameModel, _gameView,
            cardId, _topDiscardCardId,
            fromPos, toPos
        );

        // 交给管理器处理 (它会自动调用 execute)
        _undoManager->addCommand(std::move(cmd));


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