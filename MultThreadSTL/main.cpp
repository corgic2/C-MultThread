#include <functional>
#include <iostream>
#include "Charpter2/CreateThread.h"
#include "Charpter3/ThreadMutexAssistant.h"
#include "Charpter4/ThreadSynchronizationClass.h"
#define CHARPATER2 0
#define CHARPATER3 0
#define CHARPATER4 1
int main(int argc, char** argv)
{
#if CHARPATER2
    {
        TempWorkTask work;
        CreateThread charPter2;

        // 使用 std::bind 或 Lambda 表达式绑定成员函数
        charPter2.CreateThreadBJoin(true, std::bind(&TempWorkTask::doBaseThing, &work));
        charPter2.CreateThreadInVariableFunction(std::bind(&TempWorkTask::doVariableThing, &work, std::placeholders::_1));
        charPter2.CreateThreadByClassFunction();
        charPter2.CreateThreadCopyObject();
        charPter2.CreateThreadVariableNullptr();
    }
#endif
#if CHARPATER3
    {
        ThreadMutexAssistant t;
        //std::thread tmp(&ThreadMutexAssistant::FunctionToLockGuardTestForThread, &t);
        //tmp.join();
        std::thread tmp(&ThreadMutexAssistant::DeadLockTest, &t);
        tmp.join();
    }
#endif
#if CHARPATER4
    {
        ThreadSynchronizationClass t;
        std::thread tmp(&ThreadSynchronizationClass::TestCreatorComsumerModel, &t);
        tmp.join();
    }
#endif
    std::cout << "Main Function End" << std::endl;
    return 0;
}