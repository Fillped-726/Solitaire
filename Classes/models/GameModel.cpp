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
    // 必须清空牌堆序列，防止上一局的数据残留
    _drawStackIds.clear();
    _topDiscardCardId = -1;
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

    // 序列化时建议保存当前牌堆的 ID 序列，以便恢复存档时保持牌堆顺序
    // 这里暂时省略，根据您实际需求添加

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

void GameModel::initStandardDeck() {
    // 1. 清空数据
    this->reset();

    int idCounter = 0;
    // 2. 仅负责创建 52 张卡牌对象
    for (int s = 0; s <= 3; ++s) {
        for (int f = 1; f <= 13; ++f) {
            auto card = CardModel::create(
                idCounter++,
                static_cast<CardFace>(f),
                static_cast<CardSuit>(s)
            );

            // 默认状态设为 Deck，但**不要**在这里加入 _drawStackIds
            // 具体的归属（在牌桌还是在备用堆）应由 Controller 根据 LevelConfig 决定
            card->setState(CardState::Deck);
            _allCards.pushBack(card);
        }
    }
    CCLOG("GameModel: initStandardDeck generated %zd cards.", _allCards.size());
}

int GameModel::getPlayfieldCardCount() const {
    int count = 0;
    for (auto card : _allCards) {
        if (card->getState() == CardState::Playfield) {
            count++;
        }
    }
    return count;
}

void GameModel::pushToDrawStack(int cardId) {
    // 这里是 Controller 初始化关卡时调用的地方
    _drawStackIds.push_back(cardId);
}

int GameModel::popNextDrawCard() {
    if (_drawStackIds.empty()) return -1;

    // 按照原来的逻辑：取第一个元素作为下一张牌
    int id = _drawStackIds.front();
    _drawStackIds.erase(_drawStackIds.begin());
    return id;
}

int GameModel::getDrawStackSize() const {
    return (int)_drawStackIds.size();
}

void GameModel::pushBackToDrawStackTop(int cardId) {
    // 撤销逻辑：插回到最前面
    _drawStackIds.insert(_drawStackIds.begin(), cardId);
}