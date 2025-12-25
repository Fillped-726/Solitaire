#pragma once
#include "cocos2d.h"
#include <vector>

// 保持纯粹的数据定义，不包含逻辑
struct CardConfigData {
    int cardFace;       // 1-13 (A-K)
    int cardSuit;       // 1-4 (Spade, Heart, Club, Diamond)
    float x;            // 归一化坐标或像素坐标
    float y;
    int zOrder;         // 初始层级
    bool isFaceUp;      // 初始是否正面朝上
};

class LevelConfig {
public:
    int levelId;
    std::vector<CardConfigData> playfieldCards; // 主牌桌的牌
    std::vector<CardConfigData> drawStackCards; // 备用牌堆的牌

    // 简单的合法性检查
    bool isValid() const { return !playfieldCards.empty(); }
};