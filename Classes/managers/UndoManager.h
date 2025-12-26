#pragma once
#include "ICommand.h"
#include <vector>
#include <memory> 

/**
 * @class UndoManager
 * @brief 撤销/回退管理器
 * @responsibility 维护命令历史栈，负责命令的执行、入栈管理以及撤销操作。
 * @usage 由 GameController 持有成员变量，不使用单例模式。
 */
class UndoManager {
public:
    UndoManager();
    ~UndoManager();

    /**
     * @brief 执行并记录命令
     * @param cmd 命令对象的智能指针 (所有权将被转移给 UndoManager)
     * @note 调用此方法时会自动触发 cmd->execute()。
     */
    void addCommand(std::unique_ptr<ICommand> cmd);

    /**
     * @brief 执行回退操作
     * 取出栈顶命令执行 undo()，然后将其移除。
     */
    void undo();

    /**
     * @brief 清空历史记录
     * 通常在重新开始游戏或切换关卡时调用。
     */
    void clear();

private:
    /**
     * @brief 命令历史栈
     * 使用 vector 模拟栈结构，存储多态指针，自动管理生命周期。
     */
    std::vector<std::unique_ptr<ICommand>> _commandStack;
};