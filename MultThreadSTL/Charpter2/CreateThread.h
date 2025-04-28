#pragma once
#include <functional>

class TempWorkTask
{
public:
    TempWorkTask();
    ~TempWorkTask();
    TempWorkTask(const TempWorkTask& tmp);
    void doBaseThing();
    void doVariableThing(int& cnt);
};
//线程管控
class CreateThread
{
public:
    CreateThread();
    ~CreateThread();
    //基本的发起线程
    void CreateThreadBJoin(bool bJoin, std::function<void()>);
    //测试参数改变函数
    void CreateThreadInVariableFunction(std::function<void(int&)>);
    //测试类成员函数至线程
    void CreateThreadByClassFunction();
    //观察线程内部拷贝参数过程
    void CreateThreadCopyObject();
};
