#pragma once
#include "cocos2d.h"
#include "CardView.h"

class GameView : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(GameView);

    virtual bool init();

    // 供 Controller 调用：在界面上添加一张卡牌
    void addCardToLayer(CardView* cardView, const cocos2d::Vec2& pos, int zOrder);

    // onComplete: 动画播放完后的回调（用于清理数据或检查游戏结束）
    void playMoveCardAnim(int cardId, const cocos2d::Vec2& targetPos, const std::function<void()>& onComplete);

    // 辅助：根据 ID 找到 CardView 节点
    CardView* getCardViewById(int cardId);

private:
    // 布局容器节点
    cocos2d::Node* _playfieldLayer; // 主牌区
    cocos2d::Node* _stackLayer;     // 堆牌区
};