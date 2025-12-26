#pragma once
#include "managers/ICommand.h"
#include "models/GameModel.h"
#include "views/GameView.h"

/**
 * @class MoveCardCommand
 * @brief 移牌命令
 * @responsibility 处理将牌从桌面(Playfield)移动到底牌堆(Discard)的逻辑及撤销。
 */
class MoveCardCommand : public ICommand {
public:
    MoveCardCommand(GameModel* model, GameView* view,
        int cardId, int oldTopCardId,
        cocos2d::Vec2 fromPos, cocos2d::Vec2 toPos);

    void execute() override;
    void undo() override;

private:
    GameModel* _model;
    GameView* _view;

    int _cardId;        ///< 被移动的牌 ID
    int _oldTopCardId;  ///< 移动前的底牌堆顶 ID

    cocos2d::Vec2 _fromPos; ///< 起点坐标
    cocos2d::Vec2 _toPos;   ///< 终点坐标

    CardState _prevState;   ///< 移动前的逻辑状态
};