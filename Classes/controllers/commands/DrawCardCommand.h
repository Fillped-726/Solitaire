#pragma once
#include "managers/ICommand.h"
#include "models/GameModel.h"
#include "views/GameView.h"

/**
 * @class DrawCardCommand
 * @brief 翻牌命令
 * @responsibility 处理从备用牌堆(Deck)抽取一张牌到底牌堆(Discard)的逻辑及其撤销。
 * @usage 当用户点击备用牌堆时创建并执行。
 */
class DrawCardCommand : public ICommand {
public:
    /**
     * @brief 构造函数
     * @param model 数据模型
     * @param view 视图引用
     * @param cardId 被翻出的牌 ID
     * @param oldTopCardId 翻牌前底牌堆顶部的牌 ID (用于计算层级)
     */
    DrawCardCommand(GameModel* model, GameView* view, int cardId, int oldTopCardId);

    /**
     * @brief 执行翻牌
     * 将牌从 Deck 移至 Discard，并播放翻牌动画。
     */
    void execute() override;

    /**
     * @brief 撤销翻牌
     * 将牌放回 Deck 顶部，恢复隐形状态。
     */
    void undo() override;

private:
    GameModel* _model;
    GameView* _view;
    int _cardId;       ///< 被操作的卡牌ID
    int _oldTopCardId; ///< 旧的堆顶ID

    // 建议：坐标应由 Config 提供，这里暂时作为私有常量
    const cocos2d::Vec2 kDiscardPilePos = cocos2d::Vec2(540, 300);
    const cocos2d::Vec2 kDeckPilePos = cocos2d::Vec2(200, 300);
};