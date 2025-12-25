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
};