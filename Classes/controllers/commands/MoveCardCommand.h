#pragma once
#include "managers/ICommand.h"
#include "models/GameModel.h"
#include "views/GameView.h"

class MoveCardCommand : public ICommand {
public:
    // 构造函数：捕获操作那一瞬间的所有状态
    MoveCardCommand(GameModel* model, GameView* view,
        int cardId, int oldTopCardId,
        cocos2d::Vec2 fromPos, cocos2d::Vec2 toPos);

    void execute() override;
    void undo() override;

private:
    GameModel* _model;
    GameView* _view;

    int _cardId;        // 被移动的牌
    int _oldTopCardId;  // 移动前，底牌堆顶部的牌ID (用于回退时恢复状态)

    cocos2d::Vec2 _fromPos; // 起点 (桌面)
    cocos2d::Vec2 _toPos;   // 终点 (底牌堆)

    CardState _prevState;   // 移动前的状态
};