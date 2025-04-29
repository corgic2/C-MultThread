#include "CreateThread.h"
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
TempWorkTask::TempWorkTask()
{
}

TempWorkTask::~TempWorkTask()
{
}

TempWorkTask::TempWorkTask(const TempWorkTask& tmp)
{
    std::cout << "Exec Copy Function" << std::endl;
}

void TempWorkTask::doBaseThing()
{
    std::cout << "TempWorkTask DoSomeThing Function" << std::endl;
}

void TempWorkTask::doVariableThing(int& cnt)
{
    while (cnt < 100)
    {
        std::cout << "The Variable Intergrate Cnt : " << cnt << std::endl;
        ++cnt;
    }
}

void func(const TempWorkTask&)
{
}

CreateThread::CreateThread()
{
}

CreateThread::~CreateThread()
{
}

void CreateThread::CreateThreadBJoin(bool bJoin, std::function<void()> function)
{
    std::thread t(function);
    if (bJoin && t.joinable())
    {
        t.join();
    }
    else
    {
        t.detach();
    }
}

void CreateThread::CreateThreadInVariableFunction(std::function<void(int&)> function)
{
    int cnt = 10;
    std::cout << "Enter Thread Before Cnt = " << cnt << std::endl;
    std::thread t(function, std::ref(cnt));
    t.join();
    std::cout << "Thread Id is :" << t.get_id() << std::endl;
    std::cout << "Exit Thread After Cnt = " << cnt << std::endl;
}

void CreateThread::CreateThreadByClassFunction()
{
    std::cout << "CreateThreadByClassFunction Begin" << std::endl;
    TempWorkTask work;
    std::thread t(&TempWorkTask::doBaseThing, &work);
    t.join();
    int cnt = 10;
    std::function<void(int&)> function = std::bind(&TempWorkTask::doVariableThing, &work, std::placeholders::_1);
    std::thread t1(function, std::ref(cnt));
    t1.join();
    std::cout << "CreateThreadByClassFunction End" << std::endl;
}

void CreateThread::CreateThreadCopyObject()
{
    TempWorkTask work;
    std::thread t(func, work);
    t.join();
}

void CreateThread::CreateThreadVariableNullptr()
{
    TempWorkTask work;
    //auto p = new int(30);
    auto p = std::make_shared<int>(30);
    std::thread t(&TempWorkTask::doVariableThing, &work, std::ref(*p));
    //delete p;
    //p = nullptr;
    t.join();
}