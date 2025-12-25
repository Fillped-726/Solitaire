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
    
    // 1. 无论是否有图片，先初始化父类
    if (!Sprite::init()) {
        return false;
    }

    _modelRef = model;
    _cardId = model->getId();
    _isFaceUp = model->isFaceUp();

    // 2. 【强制显色】创建一个 150x200 的红色色块
    // 这样我们就不用担心 Resources 里的图片路径对不对了
    auto colorLayer = LayerColor::create(Color4B::RED, 150, 200);
    colorLayer->setPosition(Vec2(-75, -100)); // 居中偏移
    this->addChild(colorLayer);

    // 3. 设置触摸区域大小 (关键！否则点不到)
    this->setContentSize(Size(150, 200));

    // 4. 触摸监听
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    return true;
}

void CardView::updateView() {
    if (!_modelRef) return;

    // 状态同步：如果 Model 变了，View 也要变
    bool modelFaceUp = _modelRef->isFaceUp();

    std::string imgPath;
    if (modelFaceUp) {
        imgPath = CardResConfig::getCardImagePath(_modelRef->getSuit(), _modelRef->getFace());
    }
    else {
        imgPath = CardResConfig::getBackImagePath();
    }

    // 重新设置纹理
    this->setTexture(imgPath);
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
        // 视觉反馈：稍微变暗或缩放，让用户感觉“按下了”
        this->setScale(0.95f);
        return true; // 声明消费此事件
    }
    return false;
}

void CardView::onTouchEnded(Touch* touch, Event* event) {
    // 恢复缩放
    this->setScale(1.0f);

    // 触发回调：View 的任务完成了，剩下的交给 Controller
    if (_onClickCallback) {
        // [Cite: 55, 67] View 捕获事件 -> 触发回调 -> Controller 处理
        _onClickCallback(_cardId);
    }
}