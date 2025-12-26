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

    // 1. 背景
    auto bg = LayerColor::create(Color4B(50, 100, 50, 255)); // 深绿色背景
    this->addChild(bg, -1);

    // 2. 区域划分
    // 主牌区 (Playfield): 放在上方 (0, 580)
    _playfieldLayer = Node::create();
    _playfieldLayer->setPosition(Vec2(0, 580));
    this->addChild(_playfieldLayer, 1);

    // 堆牌区 (Stack): 放在底部 (0, 0)
    _stackLayer = Node::create();
    _stackLayer->setPosition(Vec2(0, 0));
    this->addChild(_stackLayer, 1);

    return true;
}

void GameView::addCardToLayer(CardView* cardView, const Vec2& pos, int zOrder) {
    if (!cardView) return;

    // 防止重复添加导致报错
    if (cardView->getParent()) {
        cardView->removeFromParent();
    }

    // 设置位置和层级
    cardView->setPosition(pos);
    cardView->setLocalZOrder(zOrder);

    // 保持您的原始逻辑：默认添加到 PlayfieldLayer
    // 注意：这意味着传入的 pos 必须是相对于 PlayfieldLayer 的坐标
    _playfieldLayer->addChild(cardView);
}

CardView* GameView::getCardViewById(int cardId) {
    // 1. 遍历主牌区
    for (auto node : _playfieldLayer->getChildren()) {
        auto card = dynamic_cast<CardView*>(node);
        if (card && card->getCardId() == cardId) return card;
    }

    // 2. [补全] 遍历堆牌区
    // 虽然目前 addCardToLayer 只加到了 Playfield，但补全这个逻辑可以防止未来扩展时的崩溃
    if (_stackLayer) {
        for (auto node : _stackLayer->getChildren()) {
            auto card = dynamic_cast<CardView*>(node);
            if (card && card->getCardId() == cardId) return card;
        }
    }

    return nullptr;
}

void GameView::playMoveCardAnim(int cardId, const Vec2& targetPos, const std::function<void()>& onComplete) {
    auto cardView = getCardViewById(cardId);
    if (!cardView) return;

    // 1. 获取当前缩放值
    float originalScale = cardView->getScale();

    // 提权：保证动画过程中在最上层
    cardView->setLocalZOrder(100);

    // 2. 动画序列
    auto moveTo = MoveTo::create(0.3f, targetPos);

    // 缩放效果：微弱放大后还原
    auto scaleUp = ScaleTo::create(0.1f, originalScale * 1.1f);
    auto scaleDown = ScaleTo::create(0.2f, originalScale);
    auto scaleSeq = Sequence::create(scaleUp, scaleDown, nullptr);

    auto anim = Spawn::create(moveTo, scaleSeq, nullptr);

    auto callback = CallFunc::create([cardView, onComplete, targetPos]() {
        // 修正最终位置
        cardView->setPosition(targetPos);

        // [关键修正] 删除了 cardView->setLocalZOrder(5);
        // 原因：层级应该由 Command 在 onComplete 执行后，通过 Model 数据或逻辑来决定。
        // 如果这里强行设为 5，会覆盖 Command 计算出的正确层级（例如 10），导致遮挡错误。

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
}