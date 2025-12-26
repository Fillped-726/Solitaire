#pragma once
#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "managers/UndoManager.h"

class GameController : public cocos2d::Ref {
public:
    // 工厂方法：创建一个控制场景
    // 注意：不再是 GameView::createScene，而是 Controller 来组装 Scene
    static cocos2d::Scene* createGameScene();

    static GameController* create(GameView* view, GameModel* model);

    // 核心交互：处理卡牌点击 [cite: 65, 66]
    void handleCardClick(int cardId);

    // 游戏流程：开始游戏
    void startGame();

    // 处理回退按钮点击
    void handleUndo(); 

    // 弃牌堆
    void handleDrawPileClick();

    // 状态检查
    void checkGameState();

    // 胜利结算
    void showGameOverUI(bool win);

private:
    bool init(GameView* view, GameModel* model);

    // 弱引用：View 和 Model 的生命周期由 Scene 或外部管理，Controller 只是协调者
    // 但为了方便，我们这里让 Controller 持有它们的引用计数
    GameView* _gameView;
    GameModel* _gameModel;


    std::unique_ptr<UndoManager> _undoManager;
};