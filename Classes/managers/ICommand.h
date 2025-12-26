#pragma once

/**
 * @interface ICommand
 * @brief 命令模式抽象接口
 * @responsibility 定义所有游戏操作（如移动、翻牌）的通用行为标准，支持执行和撤销。
 * @usage 具体命令类（如 MoveCardCommand）需继承此接口并实现纯虚函数。
 */
class ICommand {
public:
    /**
     * @brief 虚析构函数
     * 确保通过基类指针删除派生类对象时能正确释放资源。
     */
    virtual ~ICommand() {}

    /**
     * @brief 执行命令
     * 包含改变 Model 数据和更新 View 视图的逻辑。
     */
    virtual void execute() = 0;

    /**
     * @brief 撤销命令 (核心需求)
     * 将 Model 数据和 View 视图恢复到执行前的状态。
     */
    virtual void undo() = 0;
};