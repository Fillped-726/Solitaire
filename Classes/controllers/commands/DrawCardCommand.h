#pragma once
#include "managers/ICommand.h"
#include "models/GameModel.h"
#include "views/GameView.h"

class DrawCardCommand : public ICommand {
public:
    // 构造函数：需要知道是谁被翻出来了
    DrawCardCommand(GameModel* model, GameView* view, int cardId, int oldTopCardId);

    void execute() override;
    void undo() override;

private:
    GameModel* _model;
    GameView* _view;
    int _cardId;       // 被翻的那张牌
    int _oldTopCardId; // 旧的底牌 ID
};