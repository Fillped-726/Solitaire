#include "MoveCardCommand.h"

USING_NS_CC;

MoveCardCommand::MoveCardCommand(GameModel* model, GameView* view,
    int cardId, int oldTopCardId,
    Vec2 fromPos, Vec2 toPos)
    : _model(model), _view(view),
    _cardId(cardId), _oldTopCardId(oldTopCardId),
    _fromPos(fromPos), _toPos(toPos)
{
    // 只记录逻辑状态，不记录 View 层的 ZOrder
    auto card = _model->getCardById(_cardId);
    if (card) {
        _prevState = card->getState();
    }
}

void MoveCardCommand::execute() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据变更
    card->setState(CardState::Discard);
    _model->setTopCardId(_cardId);

    // 2. 视图表现
    int targetZ = 10;
    auto oldBaseView = _view->getCardViewById(_oldTopCardId);
    if (oldBaseView) {
        targetZ = oldBaseView->getLocalZOrder() + 1;
    }

    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        // 飞行时设为最高，防止被其他牌遮挡
        cardView->setLocalZOrder(1000);
    }

    // 播放动画
    _view->playMoveCardAnim(_cardId, _toPos, [this, targetZ]() {
        auto v = _view->getCardViewById(_cardId);
        // 动画结束，设置为底牌堆叠的层级
        if (v) v->setLocalZOrder(targetZ);
        });

    CCLOG("CMD: Move Card %d -> Discard", _cardId);
}

void MoveCardCommand::undo() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据恢复
    card->setState(_prevState);
    _model->setTopCardId(_oldTopCardId);

    // 2. 视图恢复
    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        // 飞行时临时提权
        cardView->setLocalZOrder(1000);
    }

    // 3. 动画飞回
    _view->playMoveCardAnim(_cardId, _fromPos, [this, card, cardView]() {
        if (cardView && card) {
            cardView->setLocalZOrder(card->getZOrder());
        }
        });

    CCLOG("CMD: Undo Move Card %d", _cardId);
}