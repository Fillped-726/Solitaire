#pragma once
#include "LevelConfig.h"
#include "json/document.h" // 需要引入 rapidjson 头文件以支持私有方法的参数类型
#include <string>

/**
 * @class LevelConfigLoader
 * @brief 关卡配置加载器
 * * @func 读取 JSON 配置文件并解析为 LevelConfig 对象。
 * @responsibility 处理文件 I/O 和 JSON 解析逻辑。
 * @usage 静态调用 loadLevel 方法。
 */
class LevelConfigLoader {
public:
    /**
     * @brief 读取 JSON 文件并返回配置对象
     * @param filename 配置文件路径 (相对于 Resources 目录)
     * @return LevelConfig 解析后的配置对象
     */
    static LevelConfig loadLevel(const std::string& filename);

private:
    /**
     * @brief 解析单个卡牌数据的私有辅助方法
     * @param item rapidjson 的 Value 对象
     * @return CardConfigData 解析出的卡牌数据结构
     */
    static CardConfigData _parseCardData(const rapidjson::Value& item);
};