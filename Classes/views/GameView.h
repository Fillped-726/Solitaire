#pragma once
#include "cocos2d.h"
#include "CardView.h"

/**
 * @class GameView
 * @brief 游戏主场景视图层
 * @responsibility 管理游戏内的两个主要图层（主牌区和堆牌区），并提供卡牌增删和动画接口。
 */
class GameView : public cocos2d::Layer {
public:
    static cocos2d::Scene* createScene();
    CREATE_FUNC(GameView);

    virtual bool init();

    /**
     * @brief 添加卡牌到显示层
     * @param cardView 卡牌视图对象
     * @param pos 相对坐标
     * @param zOrder 渲染层级
     */
    void addCardToLayer(CardView* cardView, const cocos2d::Vec2& pos, int zOrder);

    /**
     * @brief 播放卡牌移动动画
     * @param cardId 卡牌ID
     * @param targetPos 目标位置
     * @param onComplete 动画完成回调
     */
    void playMoveCardAnim(int cardId, const cocos2d::Vec2& targetPos, const std::function<void()>& onComplete);

    /**
     * @brief 根据ID查找卡牌
     * 会依次遍历 Playfield 和 Stack 图层。
     */
    CardView* getCardViewById(int cardId);

    /**
     * @brief 清空棋盘
     */
    void clearBoard();

private:
    cocos2d::Node* _playfieldLayer; ///< 主牌区容器 (上方)
    cocos2d::Node* _stackLayer;     ///< 堆牌区容器 (下方)
};