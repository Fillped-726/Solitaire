#include "UndoManager.h"
#include "cocos2d.h"

UndoManager::UndoManager() {}
UndoManager::~UndoManager() { clear(); }

void UndoManager::addCommand(std::unique_ptr<ICommand> cmd) {
    // 1. ÏÈÖ´ÐÐ
    cmd->execute();
    // 2. ÔÙÑ¹Õ»
    _commandStack.push_back(std::move(cmd));
    CCLOG("UndoManager: Command added. Stack size: %zd", _commandStack.size());
}

void UndoManager::undo() {
    if (_commandStack.empty()) {
        CCLOG("UndoManager: Nothing to undo.");
        return;
    }

    // 1. È¡³öÕ»¶¥ÃüÁî
    auto& cmd = _commandStack.back();
    // 2. Ö´ÐÐ³·Ïú
    cmd->undo();
    // 3. ÒÆ³ý
    _commandStack.pop_back();

    CCLOG("UndoManager: Undo executed. Stack size: %zd", _commandStack.size());
}

void UndoManager::clear() {
    _commandStack.clear();
}