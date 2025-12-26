#include "UndoManager.h"
#include "cocos2d.h"

UndoManager::UndoManager() {}

UndoManager::~UndoManager() {
    clear();
}

void UndoManager::addCommand(std::unique_ptr<ICommand> cmd) {
    if (!cmd) return;

    // 1. 先执行业务逻辑
    cmd->execute();

    // 2. 再压入历史栈 (使用 std::move 转移所有权)
    _commandStack.push_back(std::move(cmd));

    CCLOG("UndoManager: Command added. Stack size: %zd", _commandStack.size());
}

void UndoManager::undo() {
    if (_commandStack.empty()) {
        CCLOG("UndoManager: Nothing to undo.");
        return;
    }

    // 1. 获取栈顶命令的引用
    auto& cmd = _commandStack.back();

    // 2. 执行撤销逻辑
    if (cmd) {
        cmd->undo();
    }

    // 3. 移除记录 (此时 unique_ptr 出栈，自动销毁 Command 对象)
    _commandStack.pop_back();

    CCLOG("UndoManager: Undo executed. Stack size: %zd", _commandStack.size());
}

void UndoManager::clear() {
    // vector::clear 会销毁所有 unique_ptr，进而 delete 所有 Command 对象
    _commandStack.clear();
}