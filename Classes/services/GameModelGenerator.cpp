#include "GameModelGenerator.h"

USING_NS_CC;

void GameModelGenerator::generate(GameModel* model, const LevelConfig& config) {
    if (!model) return;

    // 1. 初始化标准 52 张牌对象池
    model->initStandardDeck();

    // 2. 先将所有牌标记为 Removed (默认不参与游戏)
    for (auto card : model->getAllCards()) {
        card->setState(CardState::Removed);
    }

    // 3. 分步组装数据
    _setupPlayfield(model, config);
    _setupDrawStack(model, config);
}

CardModel* GameModelGenerator::_findCard(GameModel* model, int face, int suit) {
    auto& allCards = model->getAllCards();
    for (auto card : allCards) {
        if ((int)card->getFace() == face && (int)card->getSuit() == suit) {
            return card;
        }
    }
    return nullptr;
}

void GameModelGenerator::_setupPlayfield(GameModel* model, const LevelConfig& config) {
    for (const auto& cfgData : config.playfieldCards) {
        CardModel* card = _findCard(model, cfgData.cardFace, cfgData.cardSuit);
        if (card) {
            card->setState(CardState::Playfield);
            card->setPosition(Vec2(cfgData.x, cfgData.y));
            card->setZOrder(cfgData.zOrder);
            // 初始翻面状态由 Config 决定 (通常第一排是正面的)
            card->setFaceUp(cfgData.isFaceUp);
        }
    }
}

void GameModelGenerator::_setupDrawStack(GameModel* model, const LevelConfig& config) {
    bool isFirst = true;

    // 约定：drawStackCards 配置列表中的第一个元素作为“底牌”，其余作为“备用牌堆”
    for (const auto& cfgData : config.drawStackCards) {
        CardModel* card = _findCard(model, cfgData.cardFace, cfgData.cardSuit);
        if (!card) continue;

        if (isFirst) {
            // 设置初始底牌 (Base Card)
            card->setState(CardState::Discard);
            card->setFaceUp(true);
            // 底牌位置固定，建议将来从 View/Config 获取常量
            card->setPosition(Vec2(540, 300));

            model->setTopCardId(card->getId());
            isFirst = false;
        }
        else {
            // 其余牌进入备用牌堆
            card->setState(CardState::Deck);
            card->setFaceUp(false);

            // [关键] 必须将 ID 加入有序列表，否则 popNextDrawCard 无法工作
            model->pushToDrawStack(card->getId());
        }
    }
}