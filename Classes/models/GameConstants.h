#pragma once

enum class CardSuit {
    None = -1,
    Clubs = 0,      // 梅花
    Diamonds = 1,   // 方块
    Hearts = 2,     // 红桃
    Spades = 3      // 黑桃
};

// 对应文档中的 CardFaceType 
enum class CardFace {
    None = -1,
    Ace = 1,
    Two = 2, Three = 3, Four = 4, Five = 5,
    Six = 6, Seven = 7, Eight = 8, Nine = 9,
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13
};

//  卡牌状态枚举
enum class CardState {
    Deck,       // 在备用牌堆
    Playfield,  // 在主牌桌（七列中）
    Discard,     // 在废弃堆（底牌区/手牌区）
    Removed        // 表示这张牌被移出游戏了
};

// 辅助：判断颜色（红/黑）
inline bool isCardRed(CardSuit suit) {
    return suit == CardSuit::Diamonds || suit == CardSuit::Hearts;
}