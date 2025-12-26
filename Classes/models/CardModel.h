#pragma once
#include "cocos2d.h"
#include "GameConstants.h"
#include "json/document.h" 

/**
 * @class CardModel
 * @brief 单张卡牌的数据模型
 * @responsibility 维护卡牌的基础属性（花色、点数）、状态（正反面、位置）以及序列化逻辑。
 * @usage 由 GameModel 统一创建和管理，View 层只读取数据不修改数据。
 */
class CardModel : public cocos2d::Ref {
public:
    /**
     * @brief 静态工厂方法创建卡牌
     * @param id 唯一 ID
     * @param face 点数
     * @param suit 花色
     * @return CardModel* 自动管理的实例对象
     */
    static CardModel* create(int id, CardFace face, CardSuit suit);

    // Getters
    int getId() const { return _id; }
    CardFace getFace() const { return _face; }
    CardSuit getSuit() const { return _suit; }

    /**
     * @brief 获取是否正面朝上
     */
    bool isFaceUp() const { return _isFaceUp; }

    /**
     * @brief 设置卡牌翻面状态
     * @param isFaceUp true为正面，false为背面
     */
    void setFaceUp(bool isFaceUp) { _isFaceUp = isFaceUp; }

    /**
     * @brief 获取卡牌当前逻辑状态（在牌堆、在牌桌等）
     */
    CardState getState() const { return _state; }

    /**
     * @brief 修改卡牌逻辑状态
     * @param state 新状态
     */
    void setState(CardState state) { _state = state; }

    /**
     * @brief 序列化：将当前状态写入 JSON 对象
     * @param outValue 输出的 RapidJSON Value
     * @param allocator RapidJSON 分配器
     */
    void toJson(rapidjson::Value& outValue, rapidjson::Document::AllocatorType& allocator) const;

    /**
     * @brief 反序列化：从 JSON 恢复数据
     * @param jsonValue 输入的 JSON 数据
     * @return bool 是否成功
     */
    bool fromJson(const rapidjson::Value& jsonValue);

    // 逻辑位置数据 (非渲染坐标，用于逻辑计算或恢复位置)
    void setPosition(const cocos2d::Vec2& pos) { _position = pos; }
    const cocos2d::Vec2& getPosition() const { return _position; }

    void setZOrder(int z) { _zOrder = z; }
    int getZOrder() const { return _zOrder; }

private:
    /**
     * @brief 内部初始化函数
     */
    bool init(int id, CardFace face, CardSuit suit);

    int _id;                 ///< 唯一标识符
    CardFace _face;          ///< 点数
    CardSuit _suit;          ///< 花色
    bool _isFaceUp;          ///< 正反面状态
    CardState _state;        ///< 当前逻辑区域
    cocos2d::Vec2 _position; ///< 逻辑坐标缓存
    int _zOrder;             ///< 逻辑层级
};