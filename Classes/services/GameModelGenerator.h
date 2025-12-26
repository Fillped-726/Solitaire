#pragma once
#include "models/GameModel.h"
#include "configs/LevelConfig.h"

/**
 * @class GameModelGenerator
 * @brief 游戏数据生成器
 * @responsibility 根据静态 LevelConfig 配置，组装并填充 GameModel 的初始状态。
 */
class GameModelGenerator {
public:
    /**
     * @brief 生成游戏关卡数据
     * @param model 目标数据模型 (将被重置并填充)
     * @param config 关卡配置数据
     */
    static void generate(GameModel* model, const LevelConfig& config);

private:
    // 辅助函数：根据花色点数查找卡牌对象
    static CardModel* _findCard(GameModel* model, int face, int suit);

    // 辅助步骤：处理主牌桌布局
    static void _setupPlayfield(GameModel* model, const LevelConfig& config);

    // 辅助步骤：处理备用牌堆与底牌
    static void _setupDrawStack(GameModel* model, const LevelConfig& config);
};