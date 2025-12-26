#pragma once
#include "models/GameModel.h"
#include "configs/LevelConfig.h"

class GameModelGenerator {
public:
    // 将配置应用到模型上
    static void generate(GameModel* model, const LevelConfig& config);
};