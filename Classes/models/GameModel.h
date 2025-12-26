#pragma once
#include "cocos2d.h"
#include "CardModel.h"
#include <vector>
#include <string>

/**
 * @class GameModel
 * @brief 全局游戏数据模型
 * @responsibility 管理所有卡牌对象的生命周期、牌堆序列以及游戏快照序列化。
 * @usage 通常由 Controller 持有，作为单一数据源。
 */
class GameModel : public cocos2d::Ref {
public:
    static GameModel* create();

    /**
     * @brief 初始化一副标准的 52 张牌
     * 会自动清空现有数据并重置牌堆。
     */
    void initStandardDeck();

    /**
     * @brief 重置游戏数据
     * 清空所有卡牌和牌堆记录。
     */
    void reset();

    /**
     * @brief 获取所有卡牌容器
     * @return cocos2d::Vector<CardModel*>& 卡牌列表引用
     */
    cocos2d::Vector<CardModel*>& getAllCards() { return _allCards; }

    /**
     * @brief 根据 ID 查找卡牌
     * @param id 卡牌 ID
     * @return CardModel* 找到返回指针，未找到返回 nullptr
     */
    CardModel* getCardById(int id);

    /**
     * @brief 序列化整个游戏状态
     * @return std::string JSON 字符串
     */
    std::string serializeToJson();

    // --- 牌堆与状态管理 ---

    void setTopCardId(int id) { _topDiscardCardId = id; }
    int getTopCardId() const { return _topDiscardCardId; }

    /**
     * @brief 获取当前主牌桌上的卡牌数量
     * 用于判断游戏进度。
     */
    int getPlayfieldCardCount() const;

    /**
     * @brief 将卡牌 ID 加入备用牌堆尾部
     */
    void pushToDrawStack(int cardId);

    /**
     * @brief 从备用牌堆取出一张牌
     * @return int 卡牌 ID，若无牌则返回 -1
     */
    int popNextDrawCard();

    /**
     * @brief 获取备用牌堆剩余数量
     */
    int getDrawStackSize() const;

    /**
     * @brief [Undo] 将牌放回备用牌堆顶部
     * 用于撤销操作。
     */
    void pushBackToDrawStackTop(int cardId);

private:
    bool init();

    cocos2d::Vector<CardModel*> _allCards;  ///< 所有卡牌的持有者
    int _topDiscardCardId = -1;             ///< 当前弃牌堆顶部的卡牌ID
    std::vector<int> _drawStackIds;         ///< 备用牌堆的卡牌ID序列
};