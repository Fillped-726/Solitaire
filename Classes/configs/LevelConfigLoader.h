#pragma once
#include "LevelConfig.h"
#include <string>

class LevelConfigLoader {
public:
    // 静态服务方法：读取 JSON 文件并返回配置对象
    static LevelConfig loadLevel(const std::string& filename);
};
