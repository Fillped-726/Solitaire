#pragma once
#include "cocos2d.h"
#include "models/CardModel.h"
#include <functional>

/**
 * @class CardView
 * @brief 单张卡牌的视图节点
 * @responsibility 负责显示卡牌的纹理（正面/背面）、响应触摸事件并反馈给 Controller。
 * @usage 由 GameView 创建和管理。
 */
class CardView : public cocos2d::Sprite {
public:
    /**
     * @brief 静态工厂方法
     * @param model 卡牌数据模型（只读）
     */
    static CardView* create(const CardModel* model);

    /**
     * @brief 根据 Model 数据刷新显示
     * 更新正反面纹理、缩放等。
     */
    void updateView();

    /**
     * @brief 设置点击回调函数
     * @param callback 接收卡牌ID的回调
     */
    void setOnClickCallback(const std::function<void(int cardId)>& callback);

    int getCardId() const { return _cardId; }

private:
    bool init(const CardModel* model);

    // 触摸事件回调
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

    int _cardId;            ///< 卡牌 ID 缓存

    // [MVC修正] View 持有 Model 的只读指针
    const CardModel* _modelRef;

    std::function<void(int)> _onClickCallback;
    float _baseScale = 1.0f; ///< 基于屏幕适配的基准缩放值
};