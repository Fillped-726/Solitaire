#include "CardModel.h"

USING_NS_CC;

CardModel* CardModel::create(int id, CardFace face, CardSuit suit) {
    CardModel* model = new (std::nothrow) CardModel();
    if (model && model->init(id, face, suit)) {
        model->autorelease();
        return model;
    }
    CC_SAFE_DELETE(model);
    return nullptr;
}

bool CardModel::init(int id, CardFace face, CardSuit suit) {
    _id = id;
    _face = face;
    _suit = suit;
    _isFaceUp = false;          // 默认背面
    _state = CardState::Deck;   // 默认在牌堆
    _zOrder = 0;

    return true;
}

void CardModel::toJson(rapidjson::Value& outValue, rapidjson::Document::AllocatorType& allocator) const {
    outValue.SetObject();
    outValue.AddMember("id", _id, allocator);
    outValue.AddMember("face", (int)_face, allocator);
    outValue.AddMember("suit", (int)_suit, allocator);
    outValue.AddMember("faceUp", _isFaceUp, allocator);
    // 建议：如果需要恢复游戏，state 和 position 也应该被序列化，视需求而定
}

bool CardModel::fromJson(const rapidjson::Value& jsonValue) {
    if (!jsonValue.HasMember("id") || !jsonValue.HasMember("face")) return false;

    _id = jsonValue["id"].GetInt();
    _face = (CardFace)jsonValue["face"].GetInt();
    _suit = (CardSuit)jsonValue["suit"].GetInt();

    if (jsonValue.HasMember("faceUp")) {
        _isFaceUp = jsonValue["faceUp"].GetBool();
    }
    return true;
}