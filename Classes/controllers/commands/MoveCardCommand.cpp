#include "MoveCardCommand.h"

USING_NS_CC;

MoveCardCommand::MoveCardCommand(GameModel* model, GameView* view,
    int cardId, int oldTopCardId,
    Vec2 fromPos, Vec2 toPos)
    : _model(model), _view(view),
    _cardId(cardId), _oldTopCardId(oldTopCardId),
    _fromPos(fromPos), _toPos(toPos)
{
    // 记录原始状态，以便撤销
    auto card = _model->getCardById(_cardId);
    if (card) {
        _prevState = card->getState();
    }
}

// [执行逻辑]：和昨天写的差不多，变成底牌
void MoveCardCommand::execute() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 更新卡牌状态
    card->setState(CardState::Discard);

    // 2. [新增] 更新 Model 里的底牌记录
    _model->setTopCardId(_cardId);

    // 3. 视图动画
    _view->playMoveCardAnim(_cardId, _toPos, nullptr);

    CCLOG("CMD: Execute Move Card %d -> Discard", _cardId);
}

// [撤销逻辑]：一切反着来
void MoveCardCommand::undo() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 恢复卡牌状态
    card->setState(_prevState);

    // 2. [关键修复] 恢复旧的底牌 ID！
    _model->setTopCardId(_oldTopCardId);

    // 3. 视图动画
    _view->playMoveCardAnim(_cardId, _fromPos, nullptr);

    CCLOG("CMD: Undo Move Card %d -> Back. Top Card Restored to %d", _cardId, _oldTopCardId);
}