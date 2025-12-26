#include "GameView.h"

USING_NS_CC;

Scene* GameView::createScene() {
    auto scene = Scene::create();
    auto layer = GameView::create();
    scene->addChild(layer);
    return scene;
}

bool GameView::init() {
    if (!Layer::init()) return false;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 背景 (可选)
    auto bg = LayerColor::create(Color4B(50, 100, 50, 255)); // 深绿色背景
    this->addChild(bg, -1);

    // 2. 区域划分 [Cite: 28, 29]
    // 主牌区 (Playfield): 1080x1500
    _playfieldLayer = Node::create();
    _playfieldLayer->setPosition(Vec2(0, 580)); // 放在上方
    this->addChild(_playfieldLayer, 1);

    // 堆牌区 (Stack): 1080x580
    _stackLayer = Node::create();
    _stackLayer->setPosition(Vec2(0, 0)); // 放在底部
    this->addChild(_stackLayer, 1);


    return true;
}

void GameView::addCardToLayer(CardView* cardView, const Vec2& pos, int zOrder) {
    // 这里为了演示简单，直接加到 Playfield
    // 实际项目中 Controller 会根据 CardModel 的状态决定加到哪个 Layer
    if (cardView->getParent()) {
        cardView->removeFromParent();
    }
    cardView->setPosition(pos);
    cardView->setLocalZOrder(zOrder);
    _playfieldLayer->addChild(cardView);
}

CardView* GameView::getCardViewById(int cardId) {
    // 简单粗暴地遍历查找 (实际项目可以用 Map 优化)
    // 遍历主牌区
    for (auto node : _playfieldLayer->getChildren()) {
        auto card = dynamic_cast<CardView*>(node);
        if (card && card->getCardId() == cardId) return card;
    }
    // 遍历堆牌区 (如果有)
    // ...
    return nullptr;
}

void GameView::playMoveCardAnim(int cardId, const Vec2& targetPos, const std::function<void()>& onComplete) {
    auto cardView = getCardViewById(cardId);
    if (!cardView) return;

    // 1. [关键修复] 获取卡牌当前的缩放值 (比如 0.2)
    float originalScale = cardView->getScale();

    cardView->setLocalZOrder(100);

    auto moveTo = MoveTo::create(0.3f, targetPos);

    // 2. [关键修复] 基于原始缩放值计算动画幅度
    // 稍微放大 1.2 倍，而不是固定变成 1.2
    auto scaleUp = ScaleTo::create(0.1f, originalScale * 1.1f);
    // 恢复到原始缩放值，而不是 1.0
    auto scaleDown = ScaleTo::create(0.2f, originalScale);

    auto anim = Spawn::create(moveTo, Sequence::create(scaleUp, scaleDown, nullptr), nullptr);

    auto callback = CallFunc::create([cardView, onComplete, targetPos]() {
        cardView->setPosition(targetPos);
        cardView->setLocalZOrder(5); // 恢复层级
        if (onComplete) onComplete();
        });

    cardView->runAction(Sequence::create(anim, callback, nullptr));
}

void GameView::clearBoard() {
    if (_playfieldLayer) {
        _playfieldLayer->removeAllChildren();
    }
    if (_stackLayer) {
        _stackLayer->removeAllChildren();
    }
    // 注意：不要调用 this->removeAllChildren()，那会把背景和 layer 都删掉
}