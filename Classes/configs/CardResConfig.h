#pragma once
#include "cocos2d.h"
#include "models/GameConstants.h"
#include <string>

#pragma once
#include "cocos2d.h"
#include "models/GameConstants.h"
#include <string>

class CardResConfig {
public:
    // 获取正面图片路径
    static std::string getCardImagePath(CardSuit suit, CardFace face) {
        // 1. 转换花色 (Enum -> String)
        std::string suitStr = "";
        switch (suit) {
        case CardSuit::Clubs:    suitStr = "clubs"; break;
        case CardSuit::Diamonds: suitStr = "diamonds"; break;
        case CardSuit::Hearts:   suitStr = "hearts"; break;
        case CardSuit::Spades:   suitStr = "spades"; break;
        default: return "card_empty.png"; // 防御性编程
        }

        // 2. 转换点数 (Enum -> String)
        std::string faceStr = "";
        int faceInt = (int)face;
        if (faceInt == 1) faceStr = "A";
        else if (faceInt == 11) faceStr = "J";
        else if (faceInt == 12) faceStr = "Q";
        else if (faceInt == 13) faceStr = "K";
        else faceStr = cocos2d::StringUtils::format("%02d", faceInt); // 2-10 直接转数字字符串

        // 3. 拼接完整路径
        // 注意路径包含文件夹：res/Cards (large)/
        // 格式示例：res/Cards (large)/card_spades_Q.png
        return cocos2d::StringUtils::format("res/PNG/Cards (large)/card_%s_%s.png",
            suitStr.c_str(), faceStr.c_str());
    }

    // 获取背面图片路径
    static std::string getBackImagePath() {
        // 请确保你刚才重命名的文件在这个路径下
        return "res/PNG/Cards (large)/card_back.png";
    }
};