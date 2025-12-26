#pragma once

/**
 * @file GameConstants.h
 * @brief 游戏全局常量与枚举定义
 */

 /**
  * @enum CardSuit
  * @brief 卡牌花色枚举
  */
enum class CardSuit {
    None = -1,
    Clubs = 0,      ///< 梅花
    Diamonds = 1,   ///< 方块
    Hearts = 2,     ///< 红桃
    Spades = 3      ///< 黑桃
};

/**
 * @enum CardFace
 * @brief 卡牌点数枚举
 */
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

/**
 * @enum CardState
 * @brief 卡牌所处的游戏状态/位置
 */
enum class CardState {
    Deck,       ///< 在备用牌堆中
    Playfield,  ///< 在主牌桌（七列中）
    Discard,    ///< 在废弃堆（底牌区/手牌区）
    Removed     ///< 已移出游戏（如已收集完成）
};
