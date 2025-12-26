#pragma once
#include "models/CardModel.h"
#include <cmath>

/**
 * @class GameRuleService
 * @brief 游戏规则判定服务
 * @responsibility 提供纯静态方法，处理接龙匹配规则、死局检测以及卡牌遮挡（翻面）逻辑。
 * @usage 无状态工具类，直接调用静态方法。
 */
class GameRuleService {
public:
    /**
     * @brief 判断手牌是否可以接在目标牌后面
     * 规则：点数差的绝对值为 1。
     * @param handCard 点击的卡牌
     * @param targetCard 当前的底牌
     */
    static bool canMatch(CardModel* handCard, CardModel* targetCard) {
        if (!handCard || !targetCard) return false;

        int faceA = (int)handCard->getFace();
        int faceB = (int)targetCard->getFace();
        int diff = std::abs(faceA - faceB);
        return diff == 1; // 暂不支持 A-K 循环，仅支持算术差值
    }

    /**
     * @brief 检查当前局面是否有解
     * 用于判定游戏失败。
     */
    static bool hasAnyMove(GameModel* model) {
        if (!model) return false;

        int topId = model->getTopCardId();
        auto topCard = model->getCardById(topId);
        if (!topCard) return false;

        for (auto card : model->getAllCards()) {
            if (card->getState() == CardState::Playfield && card->isFaceUp()) {
                if (canMatch(card, topCard)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief 刷新所有卡牌的翻面状态
     * 遍历 Playfield 中的卡牌，如果被位于下方的牌遮挡，则设为背面。
     */
    static void updateFaceUpStates(GameModel* model) {
        auto& allCards = model->getAllCards();

        for (auto cardA : allCards) {
            if (cardA->getState() != CardState::Playfield) continue;

            bool isCovered = false;
            // 检查是否有任何一张牌 B 盖住了 A
            for (auto cardB : allCards) {
                if (cardA == cardB) continue;
                if (cardB->getState() != CardState::Playfield) continue;

                if (_isCoveredBy(cardA, cardB)) {
                    isCovered = true;
                    break;
                }
            }
            cardA->setFaceUp(!isCovered);
        }
    }

private:
    // 判定阈值 (根据素材尺寸调整)
    static constexpr float kCheckWidth = 100.0f;
    static constexpr float kCheckHeight = 120.0f;

    /**
     * @brief 判定 cardB 是否盖住了 cardA
     * 规则：TriPeaks 中，位于下方(Y更小)的牌会压住上方的牌。
     */
    static bool _isCoveredBy(CardModel* cardA, CardModel* cardB) {
        // 只有当 B 在 A 的下方 (Y值更小) 时，才可能构成遮挡
        if (cardB->getPosition().y < cardA->getPosition().y) {
            float diffX = std::abs(cardA->getPosition().x - cardB->getPosition().x);
            float diffY = std::abs(cardA->getPosition().y - cardB->getPosition().y);

            if (diffX < kCheckWidth && diffY < kCheckHeight) {
                return true;
            }
        }
        return false;
    }
};