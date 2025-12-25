#include "LevelConfigLoader.h"
#include "json/document.h"  // 引用 Cocos 内置的 RapidJSON
#include "json/stringbuffer.h"

USING_NS_CC;

LevelConfig LevelConfigLoader::loadLevel(const std::string& filename) {
    LevelConfig config;
    config.levelId = 0;

    // 1. 读取文件内容
    std::string jsonContent = FileUtils::getInstance()->getStringFromFile(filename);
    if (jsonContent.empty()) {
        CCLOG("Error: Level file not found: %s", filename.c_str());
        return config;
    }

    // 2. 解析 JSON
    rapidjson::Document doc;
    doc.Parse(jsonContent.c_str());

    if (doc.HasParseError()) {
        CCLOG("Error: JSON parse error in %s", filename.c_str());
        return config;
    }

    // 3. 解析主牌桌数据 (playfield)
    if (doc.HasMember("playfield") && doc["playfield"].IsArray()) {
        const rapidjson::Value& arr = doc["playfield"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            const rapidjson::Value& item = arr[i];

            CardConfigData card;
            // 严谨的类型检查
            card.cardFace = item.HasMember("face") ? item["face"].GetInt() : 1;
            card.cardSuit = item.HasMember("suit") ? item["suit"].GetInt() : 1;
            card.x = item.HasMember("x") ? item["x"].GetFloat() : 0.0f;
            card.y = item.HasMember("y") ? item["y"].GetFloat() : 0.0f;
            card.zOrder = item.HasMember("z") ? item["z"].GetInt() : 0;
            card.isFaceUp = item.HasMember("faceUp") ? item["faceUp"].GetBool() : false;

            config.playfieldCards.push_back(card);
        }
    }

    CCLOG("Level Loaded: %d cards in playfield.", (int)config.playfieldCards.size());
    return config;
}