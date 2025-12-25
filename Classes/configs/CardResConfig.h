#pragma once
#include "cocos2d.h"
#include "models/GameConstants.h"
#include <string>

class CardResConfig {
public:
    // 获取卡牌正面图片路径
    static std::string getCardImagePath(CardSuit suit, CardFace face) {
        // 假设你的资源命名规则是 "card_[suit]_[face].png"
        // 实际请根据你的 Resouces 目录下的文件名修改这里
        // 例如：suit(0-3), face(1-13)
        // 也可以使用简单的 switch-case 映射
        // 这里只是一个示例占位符，请确保 Resources 目录下有对应的图片，或者有一张 default.png
        return "res/suits/club.png";
    }

    // 获取卡牌背面图片路径
    static std::string getBackImagePath() {
        return "res/card_general.png";
    }
};