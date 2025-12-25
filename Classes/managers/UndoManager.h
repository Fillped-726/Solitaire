#pragma once
#include "ICommand.h"
#include <vector>
#include <memory> // 使用智能指针管理命令生命周期

class UndoManager {
public:
    UndoManager();
    ~UndoManager();

    // 执行并记录命令
    void addCommand(std::unique_ptr<ICommand> cmd);

    // 执行回退
    void undo();

    // 清空历史 (新开局时用)
    void clear();

private:
    // 使用 vector 当栈用，方便管理
    std::vector<std::unique_ptr<ICommand>> _commandStack;
};