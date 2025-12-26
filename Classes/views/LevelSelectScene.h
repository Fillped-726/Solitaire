#pragma once
#include "cocos2d.h"

/**
 * @class LevelSelectScene
 * @brief 关卡选择场景
 * @responsibility 展示关卡列表，点击后切换到 GameController 创建的游戏场景。
 */
class LevelSelectScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(LevelSelectScene);
};