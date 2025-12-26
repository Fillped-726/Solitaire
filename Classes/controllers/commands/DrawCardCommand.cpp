#include "DrawCardCommand.h"

USING_NS_CC;

DrawCardCommand::DrawCardCommand(GameModel* model, GameView* view, int cardId, int oldTopCardId)
    : _model(model), _view(view), _cardId(cardId), _oldTopCardId(oldTopCardId)
{
}

void DrawCardCommand::execute() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据变更
    card->setState(CardState::Discard);
    card->setFaceUp(true);
    _model->setTopCardId(_cardId);

    // 2. 视图表现
    Vec2 targetPos = Vec2(540, 300);
    auto cardView = _view->getCardViewById(_cardId);

    int targetZ = 10; // 默认基础高度
    auto oldBaseView = _view->getCardViewById(_oldTopCardId);
    if (oldBaseView) {
        // 让新牌永远比旧牌高 1 层
        targetZ = oldBaseView->getLocalZOrder() + 1;
    }

    if (cardView) {
        // [关键] 它是从隐形状态出来的，必须设为可见！
        cardView->setVisible(true);

        cardView->updateView();

        // 确保它飞的时候在最上层，飞完后盖在旧底牌上
        // 之前旧底牌Z可能是0或5，我们这里给个更高的值，比如 10
        cardView->setLocalZOrder(100);

        _view->playMoveCardAnim(_cardId, targetPos, [cardView, targetZ]() {
            // 动画结束，应用计算好的层级
            cardView->setLocalZOrder(targetZ);
            });
    }

    CCLOG("CMD: Draw Card %d", _cardId);
}

void DrawCardCommand::undo() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据恢复
    card->setState(CardState::Deck);
    card->setFaceUp(false);
    _model->setTopCardId(_oldTopCardId);

    // 2. 视图恢复
    Vec2 deckPos = Vec2(200, 300); // 飞回左下角

    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        // 飞回去
        _view->playMoveCardAnim(_cardId, deckPos, [cardView]() {
            cardView->updateView(); // 变回背面

            // [关键] 飞回牌堆后，重新变成隐形！
            // 这样看起来就像钻进了牌堆里
            cardView->setVisible(false);
            });
    }

    CCLOG("CMD: Undo Draw Card %d", _cardId);
}