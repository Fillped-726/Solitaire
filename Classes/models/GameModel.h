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

    int getPlayfieldCardCount() const {
        int count = 0;
        for (auto card : _allCards) {
            // 只统计还在 Playfield 的牌
            if (card->getState() == CardState::Playfield) {
                count++;
            }
        }
        return count;
    }

    // [新增] 牌堆序列管理
    void pushToDrawStack(int cardId) {
        _drawStackIds.push_back(cardId);
    }

    // 获取下一张要发的牌 ID (如果没有了返回 -1)
    int popNextDrawCard() {
        if (_drawStackIds.empty()) return -1;

        // 我们的逻辑是：序列的第0个是下一个要发的
        int id = _drawStackIds.front();
        _drawStackIds.erase(_drawStackIds.begin());
        return id;
    }

    // 获取当前牌堆剩余数量
    int getDrawStackSize() const {
        return (int)_drawStackIds.size();
    }

    // [Undo 专用] 把牌塞回牌堆顶部 (撤销时用)
    void pushBackToDrawStackTop(int cardId) {
        _drawStackIds.insert(_drawStackIds.begin(), cardId);
    }

private:
    bool init();
    cocos2d::Vector<CardModel*> _allCards;
    int _topDiscardCardId = -1;
    std::vector<int> _drawStackIds;
};