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

    // 1. 数据变更
    card->setState(CardState::Discard);
    _model->setTopCardId(_cardId); // 注意：这里不需要手动 setFaceUp，因为桌面牌本来就是 FaceUp

    // 2. 视图表现
    // [关键新增] 计算层级
    int targetZ = 10;
    auto oldBaseView = _view->getCardViewById(_oldTopCardId);
    if (oldBaseView) {
        targetZ = oldBaseView->getLocalZOrder() + 1;
    }

    // 飞行时设为最高
    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        cardView->setLocalZOrder(1000);
    }

    _view->playMoveCardAnim(_cardId, _toPos, [this, targetZ]() { // 捕获 targetZ
        // 动画结束，设置层级
        auto v = _view->getCardViewById(_cardId);
        if (v) v->setLocalZOrder(targetZ);
        });

    CCLOG("CMD: Move Card %d -> Discard (Z: %d)", _cardId, targetZ);
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