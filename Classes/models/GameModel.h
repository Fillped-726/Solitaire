#pragma once
#include "cocos2d.h"
#include "CardModel.h"

class GameModel : public cocos2d::Ref {
public:
    static GameModel* create();

    //  初始化一副标准的 52 张牌
    void initStandardDeck();

    // 初始化牌组数据（清空并重新生成）
    void reset();

    // 核心数据容器：使用 Cocos Vector 自动管理内存
    cocos2d::Vector<CardModel*>& getAllCards() { return _allCards; }

    // 根据 ID 查找卡牌 (O(N) 查找，对于 52 张牌足够快)
    CardModel* getCardById(int id);

    // 序列化整个游戏状态（包含所有卡牌）
    std::string serializeToJson();

    // 管理当前底牌的 ID
    void setTopCardId(int id) { _topDiscardCardId = id; }
    int getTopCardId() const { return _topDiscardCardId; }

private:
    bool init();
    cocos2d::Vector<CardModel*> _allCards;
    int _topDiscardCardId = -1;
};