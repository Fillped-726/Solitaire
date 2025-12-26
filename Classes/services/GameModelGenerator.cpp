#include "GameModelGenerator.h"

USING_NS_CC;

void GameModelGenerator::generate(GameModel* model, const LevelConfig& config) {
    if (!model) return;
    model->initStandardDeck();
    auto& allCards = model->getAllCards();

    // 辅助 lambda
    auto findCard = [&](int face, int suit) -> CardModel* {
        for (auto card : allCards) {
            if ((int)card->getFace() == face && (int)card->getSuit() == suit) {
                return card;
            }
        }
        return nullptr;
        };

    // 1. 先把所有牌都标记为 Removed (默认不通过)
    // 后面用到的再“复活”
    for (auto card : allCards) {
        card->setState(CardState::Removed);
    }

    // 2. 处理主牌区 (Playfield)
    for (const auto& cfgData : config.playfieldCards) {
        CardModel* card = findCard(cfgData.cardFace, cfgData.cardSuit);
        if (card) {
            card->setState(CardState::Playfield);
            card->setPosition(Vec2(cfgData.x, cfgData.y));
            card->setZOrder(cfgData.zOrder);
            card->setFaceUp(cfgData.isFaceUp);
        }
    }

    // 3. [核心修改] 处理备用牌堆 (DrawStack) - 按顺序！
    // 我们的逻辑：JSON 里的第一个是“底牌”，剩下的进牌堆
    bool isFirst = true;

    for (const auto& cfgData : config.drawStackCards) {
        CardModel* card = findCard(cfgData.cardFace, cfgData.cardSuit);
        if (card) {
            if (isFirst) {
                // 第一张作为初始底牌 (BaseCard)
                // 但 Generator 不负责设置 BaseCard 的位置逻辑，只标记状态
                // 我们约定：State::Discard 且 FaceUp=true 代表底牌
                card->setState(CardState::Discard);
                card->setFaceUp(true);
                card->setPosition(Vec2(540, 300));
                model->setTopCardId(card->getId());
                isFirst = false;
            }
            else {
                // 后面的进入牌堆
                card->setState(CardState::Deck);
                card->setFaceUp(false);

                // [关键] 加入有序列表！
                model->pushToDrawStack(card->getId());
            }
        }
    }
}