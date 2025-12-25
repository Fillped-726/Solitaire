#include "GameModel.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;

GameModel* GameModel::create() {
    GameModel* ret = new (std::nothrow) GameModel();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool GameModel::init() {
    return true;
}

void GameModel::reset() {
    _allCards.clear();
}

CardModel* GameModel::getCardById(int id) {
    for (auto card : _allCards) {
        if (card->getId() == id) return card;
    }
    return nullptr;
}

std::string GameModel::serializeToJson() {
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    rapidjson::Value cardsArray(rapidjson::kArrayType);
    for (auto card : _allCards) {
        rapidjson::Value cardVal;
        card->toJson(cardVal, allocator);
        cardsArray.PushBack(cardVal, allocator);
    }

    doc.AddMember("cards", cardsArray, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

void GameModel::initStandardDeck() {
    // 先清空旧数据
    this->reset();

    int idCounter = 0;
    // 遍历 4 种花色 (0-3)
    for (int s = 0; s <= 3; ++s) {
        // 遍历 13 个点数 (1-13)
        for (int f = 1; f <= 13; ++f) {
            auto card = CardModel::create(
                idCounter++,
                static_cast<CardFace>(f),
                static_cast<CardSuit>(s)
            );
            // 默认都在备用堆
            card->setState(CardState::Deck);
            _allCards.pushBack(card);
        }
    }
    CCLOG("GameModel: initStandardDeck generated %zd cards.", _allCards.size());
}