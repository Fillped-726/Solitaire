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

    // 1. 提升层级，确保飞行的牌在最上层
    cardView->setLocalZOrder(100);

    // 2. 创建动作序列
    // 0.3秒移动到目标位置
    auto moveTo = MoveTo::create(0.3f, targetPos);
    // 稍微放大一点，增加视觉打击感
    auto scaleUp = ScaleTo::create(0.1f, 1.2f);
    auto scaleDown = ScaleTo::create(0.2f, 1.0f);
    auto anim = Spawn::create(moveTo, Sequence::create(scaleUp, scaleDown, nullptr), nullptr);

    // 3. 动作结束后的回调
    auto callback = CallFunc::create([cardView, onComplete, targetPos]() {
        // 修正位置（防止浮点误差）
        cardView->setPosition(targetPos);
        // 恢复层级 (或者这一步由 Controller 决定是否销毁)
        cardView->setLocalZOrder(0);

        if (onComplete) onComplete();
        });

    cardView->runAction(Sequence::create(anim, callback, nullptr));
}