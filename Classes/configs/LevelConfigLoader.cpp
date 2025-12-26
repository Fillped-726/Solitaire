#include "LevelConfigLoader.h"
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

    // 3. 解析基础信息
    if (doc.HasMember("levelId")) {
        config.levelId = doc["levelId"].GetInt();
    }

    // 4. 解析主牌桌数据
    if (doc.HasMember("playfield") && doc["playfield"].IsArray()) {
        const rapidjson::Value& arr = doc["playfield"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.playfieldCards.push_back(_parseCardData(arr[i]));
        }
    }

    // 5. 解析备用牌堆数据
    if (doc.HasMember("drawStack") && doc["drawStack"].IsArray()) {
        const rapidjson::Value& arr = doc["drawStack"];
        for (rapidjson::SizeType i = 0; i < arr.Size(); ++i) {
            config.drawStackCards.push_back(_parseCardData(arr[i]));
        }
    }

    CCLOG("Level Loaded: Playfield count: %d, DrawStack count: %d",
        (int)config.playfieldCards.size(),
        (int)config.drawStackCards.size());

    return config;
}

CardConfigData LevelConfigLoader::_parseCardData(const rapidjson::Value& item) {
    CardConfigData card;
    // 严谨的类型检查与默认值，保证解析安全性
    card.cardFace = item.HasMember("face") ? item["face"].GetInt() : 1;
    card.cardSuit = item.HasMember("suit") ? item["suit"].GetInt() : 1;
    card.x = item.HasMember("x") ? item["x"].GetFloat() : 0.0f;
    card.y = item.HasMember("y") ? item["y"].GetFloat() : 0.0f;
    card.zOrder = item.HasMember("z") ? item["z"].GetInt() : 0;
    card.isFaceUp = item.HasMember("faceUp") ? item["faceUp"].GetBool() : false;
    return card;
}