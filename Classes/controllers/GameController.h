#pragma once
#include "cocos2d.h"
#include "models/GameModel.h"
#include "views/GameView.h"
#include "managers/UndoManager.h"

/**
 * @class GameController
 * @brief 游戏核心控制器
 * @responsibility 协调 Model 和 View，处理用户输入，管理游戏流程（开始、结束、判定）。
 * @usage 由 Scene 创建并持有，通过回调响应 View 的交互。
 */
class GameController : public cocos2d::Ref {
public:
    /**
     * @brief 析构函数，负责释放持有的 Model 和 View
     */
    virtual ~GameController();

    /**
     * @brief 工厂方法：创建并组装游戏场景
     * @param levelId 关卡 ID
     * @return cocos2d::Scene* 包含 View 和 Controller 的完整场景
     */
    static cocos2d::Scene* createGameScene(int levelId);

    /**
     * @brief 创建控制器实例
     */
    static GameController* create(GameView* view, GameModel* model);

    /**
     * @brief 游戏流程：开始游戏
     * 加载配置、初始化数据和视图。
     */
    void startGame();

    // --- 交互处理 ---

    /**
     * @brief 处理卡牌点击事件
     * @param cardId 被点击的卡牌 ID
     */
    void handleCardClick(int cardId);

    /**
     * @brief 处理回退(Undo)按钮点击
     */
    void handleUndo();

    /**
     * @brief 处理备用牌堆(Draw Pile)点击
     * 从牌堆抽取一张新牌到底牌区。
     */
    void handleDrawPileClick();

    // --- 状态管理 ---

    /**
     * @brief 检查游戏胜负状态
     * 在每步操作后调用。
     */
    void checkGameState();

    /**
     * @brief 显示游戏结束结算界面
     * @param win true为胜利，false为失败
     */
    void showGameOverUI(bool win);

private:
    bool init(GameView* view, GameModel* model);

    /**
     * @brief 刷新棋盘状态
     * 重新计算遮挡关系并更新视图显示。
     */
    void refreshBoardState();

    // --- 私有辅助函数 (用于拆分过长的逻辑) ---

    /**
     * @brief 加载关卡配置并生成 Model 数据
     * @return bool 是否加载成功
     */
    bool _loadLevel();

    /**
     * @brief 根据 Model 初始化所有卡牌的视图
     */
    void _initCardViews();

    /**
     * @brief 初始化场景UI (Undo按钮、牌堆交互区)
     */
    void _initSceneUI();

    // 成员变量
    GameView* _gameView;            ///< 游戏视图引用
    GameModel* _gameModel;          ///< 游戏数据引用
    int _currentLevelId;            ///< 当前关卡 ID
    std::unique_ptr<UndoManager> _undoManager; ///< 撤销管理器
};