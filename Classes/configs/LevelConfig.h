#pragma once
#include "cocos2d.h"
#include <vector>

/**
 * @struct CardConfigData
 * @brief 单张卡牌的初始配置数据
 * 用于序列化/反序列化关卡文件中的卡牌信息。
 */
struct CardConfigData {
    int cardFace;   ///< 牌面点数 1-13 (A-K)
    int cardSuit;   ///< 牌面花色 1-4
    float x;        ///< 初始 X 坐标
    float y;        ///< 初始 Y 坐标
    int zOrder;     ///< 初始层级 (Z-Order)
    bool isFaceUp;  ///< 初始状态是否正面朝上
};

/**
 * @class LevelConfig
 * @brief 关卡静态配置数据类
 * * @func 存储从 JSON 文件加载的关卡布局信息。
 * @responsibility 仅作为数据容器，不包含游戏逻辑。
 * @usage 由 LevelConfigLoader 加载，传递给 GameController 初始化牌局。
 */
class LevelConfig {
public:
    int levelId;                                    ///< 关卡 ID
    std::vector<CardConfigData> playfieldCards;     ///< 主牌桌上的卡牌配置列表
    std::vector<CardConfigData> drawStackCards;     ///< 备用牌堆的卡牌配置列表

    /**
     * @brief 检查配置数据是否合法
     * @return bool 如果主牌桌不为空则返回 true
     */
    bool isValid() const { return !playfieldCards.empty(); }
};