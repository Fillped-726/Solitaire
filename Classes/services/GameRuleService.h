#pragma once
#include "models/CardModel.h"
#include <cmath> // for std::abs

class GameRuleService {
public:
    // 核心规则：判断 handCard 是否可以接在 targetCard 后面
    // 规则：|faceA - faceB| == 1
    // 特殊情况：如果是接龙游戏，通常 K(13) 后面不能接 A(1)，A 后面只能接 2。
    // 文档未提及循环接龙，我们按标准算术差值处理。
    static bool canMatch(CardModel* handCard, CardModel* targetCard) {
        if (!handCard || !targetCard) return false;

        int faceA = (int)handCard->getFace();
        int faceB = (int)targetCard->getFace();

        int diff = std::abs(faceA - faceB);
        return diff == 1;
    }

    // 备用规则：是否是万能牌 (文档未具体提及万能牌逻辑，预留接口)
    static bool isWildCard(CardModel* card) {
        return false;
    }

    static bool hasAnyMove(GameModel* model) {
        if (!model) return false;

        // 1. 获取当前的底牌
        int topId = model->getTopCardId();
        auto topCard = model->getCardById(topId);
        if (!topCard) return false;

        // 2. 遍历所有还在桌面的牌
        for (auto card : model->getAllCards()) {
            // 必须是 Playfield 且是 正面 (FaceUp) 才能被点击
            if (card->getState() == CardState::Playfield && card->isFaceUp()) {
                // 如果有一张能匹配，说明还没死局
                if (canMatch(card, topCard)) {
                    return true;
                }
            }
        }

        // 找遍了都没得消 -> 死局
        return false;
    }

    // 核心算法：刷新所有卡牌的翻面状态
    // 规则：如果在 Playfield 中，卡牌 A 被卡牌 B 盖住，则 A 为背面，否则为正面。
    static void updateFaceUpStates(GameModel* model) {
        auto& allCards = model->getAllCards();

        // 设定判定阈值 (根据你的卡牌图片大小调整)
        // 假设卡牌宽约 150，高约 200
        // 如果两张牌 X 距离小于 100 且 Y 距离小于 120，认为产生了遮挡
        const float CHECK_WIDTH = 100.0f; 
        const float CHECK_HEIGHT = 120.0f;

        for (auto cardA : allCards) {
            // 只处理桌面上的牌
            if (cardA->getState() != CardState::Playfield) continue;

            bool isCovered = false;

            // 检查是否有任何一张牌 B 盖住了 A
            for (auto cardB : allCards) {
                if (cardA == cardB) continue;
                if (cardB->getState() != CardState::Playfield) continue;

                // 核心判定：TriPeaks 中，低处的牌(Y小) 盖住 高处的牌(Y大)
                // 也就是：如果 B 的 Y < A 的 Y (B在A下方)，且两者产生重叠
                if (cardB->getPosition().y < cardA->getPosition().y) {
                    
                    float diffX = std::abs(cardA->getPosition().x - cardB->getPosition().x);
                    float diffY = std::abs(cardA->getPosition().y - cardB->getPosition().y);

                    if (diffX < CHECK_WIDTH && diffY < CHECK_HEIGHT) {
                        isCovered = true;
                        break; // 只要被一张牌盖住，就废了
                    }
                }
            }

            // 更新状态
            // 如果被盖住 -> 背面 (FaceUp = false)
            // 没被盖住 -> 正面 (FaceUp = true)
            cardA->setFaceUp(!isCovered);
        }
    }
};