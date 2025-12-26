#pragma once
#include "cocos2d.h"
#include "models/GameConstants.h"
#include <string>

/**
 * @class CardResConfig
 * @brief 卡牌资源路径配置辅助类
 * * @func 提供根据卡牌花色、点数获取对应图片资源路径的静态方法。
 * @responsibility 负责资源路径的拼接逻辑，不持有任何状态。
 * @usage 在 View 层创建卡牌 Sprite 时调用。
 */
class CardResConfig {
public:
    /**
     * @brief 获取卡牌正面图片的资源路径
     * @param suit 卡牌花色 (枚举)
     * @param face 卡牌点数 (枚举)
     * @return std::string 完整的图片路径 (e.g., "res/PNG/Cards/card_spades_Q.png")
     */
    static std::string getCardImagePath(CardSuit suit, CardFace face) {
        // 1. 转换花色 (Enum -> String)
        std::string suitStr = "";
        switch (suit) {
        case CardSuit::Clubs:    suitStr = "clubs"; break;
        case CardSuit::Diamonds: suitStr = "diamonds"; break;
        case CardSuit::Hearts:   suitStr = "hearts"; break;
        case CardSuit::Spades:   suitStr = "spades"; break;
        default: return "card_empty.png";
        }

        // 2. 转换点数 (Enum -> String)
        std::string faceStr = "";
        int faceInt = (int)face;
        if (faceInt == 1) faceStr = "A";
        else if (faceInt == 11) faceStr = "J";
        else if (faceInt == 12) faceStr = "Q";
        else if (faceInt == 13) faceStr = "K";
        else faceStr = cocos2d::StringUtils::format("%02d", faceInt);

        // 3. 拼接完整路径
        return cocos2d::StringUtils::format("res/PNG/Cards (large)/card_%s_%s.png",
            suitStr.c_str(), faceStr.c_str());
    }

    /**
     * @brief 获取卡牌背面图片的资源路径
     * @return std::string 背面图片路径
     */
    static std::string getBackImagePath() {
        return "res/PNG/Cards (large)/card_back.png";
    }
};