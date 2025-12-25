#pragma once
#include "cocos2d.h"
#include "GameConstants.h"
#include "json/document.h" // 引入 RapidJSON 用于序列化

// Model 层：继承 Ref 以利用 Cocos 的引用计数管理内存
// 但绝对不包含 Sprite 或 View 逻辑 
class CardModel : public cocos2d::Ref {
public:
    static CardModel* create(int id, CardFace face, CardSuit suit);

    // Getters
    int getId() const { return _id; }
    CardFace getFace() const { return _face; }
    CardSuit getSuit() const { return _suit; }

    // 状态读写
    bool isFaceUp() const { return _isFaceUp; }
    void setFaceUp(bool isFaceUp) { _isFaceUp = isFaceUp; }

    //  状态读写接口
    CardState getState() const { return _state; }
    void setState(CardState state) { _state = state; }

    // 序列化接口 
    // 将当前状态转为 JSON 对象，用于存档
    void toJson(rapidjson::Value& outValue, rapidjson::Document::AllocatorType& allocator) const;

    // 反序列化接口
    // 从 JSON 恢复数据
    bool fromJson(const rapidjson::Value& jsonValue);

private:
    bool init(int id, CardFace face, CardSuit suit);

    int _id;                // 唯一标识符
    CardFace _face;         // 点数
    CardSuit _suit;         // 花色
    bool _isFaceUp;         // 正反面状态
    CardState _state;       //卡牌状态
};