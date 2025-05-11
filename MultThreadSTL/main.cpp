#include <functional>
#include <iostream>
#include "Charpter2/CreateThread.h"
#include "Charpter3/ThreadMutexAssistant.h"
#include "Charpter4/ThreadSynchronizationClass.h"
#include "ThreadPool/ThreadPool.h"
#define CHARPATER2 0
#define CHARPATER3 0
#define CHARPATER4 0
#define THREADPOOL 1
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
#if THREADPOOL
    {
        ThreadPool pool;
        int cnt = 0;
        std::mutex mtx;
        std::vector<std::future<int>> res;
        for (int i = 0; i < 1000; ++i)
        {
            auto tmp = pool.AddTask([i,&cnt,&mtx]()
            {
                cnt++;
                return cnt;
            });
            res.push_back(std::move(tmp));
        }
        int result = 0;
        for (auto& tmp : res)
        {
            result += tmp.get();
        }
        std::cout << "the result is " << result << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
#endif
    std::cout << "Main Function End" << std::endl;
    return 0;
}