#include "GameModelGenerator.h"

USING_NS_CC;

void GameModelGenerator::generate(GameModel* model, const LevelConfig& config) {
    if (!model) return;

    // 1. 先重置为 52 张标准牌 (全都在 Deck 状态)
    model->initStandardDeck();
    auto& allCards = model->getAllCards();

    // 辅助 lambda: 根据 face/suit 查找现有卡牌
    auto findCard = [&](int face, int suit) -> CardModel* {
        for (auto card : allCards) {
            if ((int)card->getFace() == face && (int)card->getSuit() == suit) {
                return card;
            }
        }
        return nullptr;
        };

    CCLOG("Generator: Processing Level Config...");

    // 2. 处理主牌区 (Playfield) 配置
    for (const auto& cfgData : config.playfieldCards) {
        CardModel* card = findCard(cfgData.cardFace, cfgData.cardSuit);
        if (card) {
            // 设置状态：移动到 Playfield
            card->setState(CardState::Playfield);
            card->setPosition(Vec2(cfgData.x, cfgData.y));
            card->setZOrder(cfgData.zOrder);
            card->setFaceUp(cfgData.isFaceUp);

            CCLOG(" -> Setup Card %d at (%f, %f)", card->getId(), cfgData.x, cfgData.y);
        }
    }

    // 3. 处理备用牌堆 (DrawStack) - 如果 JSON 里有配的话
    // 如果 JSON 里没配的牌，默认保留在 CardState::Deck 状态，即视为备用牌
}