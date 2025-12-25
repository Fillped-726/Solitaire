#pragma once
#include "cocos2d.h"
#include "models/CardModel.h"
#include <functional>

class CardView : public cocos2d::Sprite {
public:
    static CardView* create(CardModel* model);

    // 根据 Model 数据刷新显示（翻面、换图）
    void updateView();

    // 设置点击回调函数 (Controller 将绑定这个接口)
    void setOnClickCallback(const std::function<void(int cardId)>& callback);

    // 获取绑定的 Model ID
    int getCardId() const { return _cardId; }

    

private:
    bool init(CardModel* model);

    // 触摸事件处理
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    int _cardId;        // 缓存 ID
    bool _isFaceUp;     // 缓存当前显示状态

    // 弱引用：View 不拥有 Model，Model 由 GameModel 管理
    // 这里我们只在创建时读取一次，或者每次 updateView 读取。
    // 为了解耦，View 甚至可以不持有 Model 指针，只持有 ID 和数据快照。
    // 但为了方便 updateView，持有弱引用或裸指针（需保证 Model 活得比 View 长）是常见的。
    CardModel* _modelRef;

    std::function<void(int)> _onClickCallback;
};