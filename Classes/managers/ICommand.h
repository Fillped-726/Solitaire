#pragma once

class ICommand {
public:
    virtual ~ICommand() {}

    // 执行命令
    virtual void execute() = 0;

    // 撤销命令（核心需求）
    virtual void undo() = 0;
};