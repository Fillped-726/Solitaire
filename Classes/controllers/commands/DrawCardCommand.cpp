#include "DrawCardCommand.h"

USING_NS_CC;

DrawCardCommand::DrawCardCommand(GameModel* model, GameView* view, int cardId, int oldTopCardId)
    : _model(model), _view(view), _cardId(cardId), _oldTopCardId(oldTopCardId)
{
}

void DrawCardCommand::execute() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据变更：从牌堆移到底牌区，并设为正面
    card->setState(CardState::Discard);
    card->setFaceUp(true);
    _model->setTopCardId(_cardId);

    // 2. 视图变更：计算目标层级
    int targetZ = 10;
    auto oldBaseView = _view->getCardViewById(_oldTopCardId);
    if (oldBaseView) {
        // 保证新牌在旧牌上面
        targetZ = oldBaseView->getLocalZOrder() + 1;
    }

    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        // 关键状态切换：从不可见(Deck)变为可见
        cardView->setVisible(true);
        cardView->updateView(); // 刷新贴图

        // 动画过程设为高层级，防止穿插
        cardView->setLocalZOrder(100);

        _view->playMoveCardAnim(_cardId, kDiscardPilePos, [cardView, targetZ]() {
            // 动画结束后，设定为正确的堆叠层级
            cardView->setLocalZOrder(targetZ);
            });
    }

    CCLOG("CMD: Draw Card %d", _cardId);
}

void DrawCardCommand::undo() {
    auto card = _model->getCardById(_cardId);
    if (!card) return;

    // 1. 数据恢复：回退到牌堆
    card->setState(CardState::Deck);
    card->setFaceUp(false);

    // 恢复旧的 Top ID
    _model->setTopCardId(_oldTopCardId);

    // 核心逻辑：将 ID 插回牌堆序列的头部 (Model 层修复后的逻辑)
    _model->pushBackToDrawStackTop(_cardId);

    // 2. 视图恢复：飞回左下角
    auto cardView = _view->getCardViewById(_cardId);
    if (cardView) {
        // 保持高层级飞行
        cardView->setLocalZOrder(100);

        _view->playMoveCardAnim(_cardId, kDeckPilePos, [cardView]() {
            cardView->updateView(); // 变回背面
            // 关键：飞回后设为不可见，模拟“进入牌堆”
            cardView->setVisible(false);
            });
    }

    CCLOG("CMD: Undo Draw Card %d", _cardId);
}