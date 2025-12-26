#include "CardView.h"
#include "configs/CardResConfig.h"

USING_NS_CC;

CardView* CardView::create(CardModel* model) {
    CardView* view = new (std::nothrow) CardView();
    if (view && view->init(model)) {
        view->autorelease();
        return view;
    }
    CC_SAFE_DELETE(view);
    return nullptr;
}

bool CardView::init(CardModel* model) {
    // 1. 初始化 Sprite (此时还没纹理，或者给个默认的)
    if (!Sprite::init()) return false;

    _modelRef = model;
    _cardId = model->getId();

    // 2. 立即刷新一次显示 (加载正确的图片)
    this->updateView();

    // 3. 触摸监听 (保持不变)
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
        // 正面
        imgPath = CardResConfig::getCardImagePath(_modelRef->getSuit(), _modelRef->getFace());
    }
    else {
        // 背面
        imgPath = CardResConfig::getBackImagePath();
    }

    this->setTexture(imgPath);

    // [关键修改] 计算缩放并保存到 _baseScale
    if (this->getContentSize().width > 0) {
        // 假设目标宽度是 180 像素 (根据你的屏幕调整)
        _baseScale = 180.0f / this->getContentSize().width;
        this->setScale(_baseScale);
    }
}

void CardView::setOnClickCallback(const std::function<void(int)>& callback) {
    _onClickCallback = callback;
}

bool CardView::onTouchBegan(Touch* touch, Event* event) {
    // 1. 坐标转换：判断点击点是否在当前 Sprite 内部
    Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
    Size s = this->getContentSize();
    Rect rect = Rect(0, 0, s.width, s.height);

    if (rect.containsPoint(locationInNode)) {
        // [关键修改] 基于 _baseScale 稍微变小一点点 (比如 95%)
        this->setScale(_baseScale * 0.95f);
        return true;
    }
    return false;
}

void CardView::onTouchEnded(Touch* touch, Event* event) {
    // [关键修改] 恢复到正常大小，而不是 1.0
    this->setScale(_baseScale);

    if (_onClickCallback) {
        _onClickCallback(_cardId);
    }
}

