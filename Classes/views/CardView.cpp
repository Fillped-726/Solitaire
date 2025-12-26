#include "CardView.h"
#include "configs/CardResConfig.h"

USING_NS_CC;

CardView* CardView::create(const CardModel* model) {
    CardView* view = new (std::nothrow) CardView();
    if (view && view->init(model)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::init(const CardModel* model) {
    if (!Sprite::init()) return false;

    _modelRef = model;
    _cardId = model->getId();

    // 立即刷新外观
    this->updateView();

    // 绑定触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void CardView::updateView() {
    if (!_modelRef) return;

    std::string imgPath;
    if (_modelRef->isFaceUp()) {
        imgPath = CardResConfig::getCardImagePath(_modelRef->getSuit(), _modelRef->getFace());
    }
    else {
        imgPath = CardResConfig::getBackImagePath();
    }

    this->setTexture(imgPath);

    // 动态计算缩放：假设标准宽度为 180px
    if (this->getContentSize().width > 0) {
        _baseScale = 180.0f / this->getContentSize().width;
        this->setScale(_baseScale);
    }
}

void CardView::setOnClickCallback(const std::function<void(int)>& callback) {
    _onClickCallback = callback;
}

bool CardView::onTouchBegan(Touch* touch, Event* event) {
    Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
    Size s = this->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);

    if (rect.containsPoint(locationInNode)) {
        // 点击反馈：轻微缩小
        this->setScale(_baseScale * 0.95f);
        return true;
    }
    return false;
}

void CardView::onTouchEnded(Touch* touch, Event* event) {
    // 恢复缩放
    this->setScale(_baseScale);

    // 触发点击逻辑
    // 注意：这里简化了逻辑，只要点了并且松开就触发，不管松开时手指是否移出了卡牌
    // 如果需要严格判定，需再次检查 rect.containsPoint
    if (_onClickCallback) {
        _onClickCallback(_cardId);
    }
}