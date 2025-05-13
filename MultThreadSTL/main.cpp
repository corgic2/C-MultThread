#include <cassert>
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
        // 并发测试
        ThreadPool pool;
        std::vector<std::future<int>> futures;
        time_t now = std::time(nullptr);
        std::cout << "ThreadPool Test Start : " << ctime(&now) << std::endl;
        for (int i = 0; i < 1e6; ++i)
        {
            //单线程预计需要1小时完成10000000次计算
            //std::thread t([&]()
            //{
            //    return i * i;
            //});
            //t.join();
            futures.push_back(pool.AddTask([i] { return i * i; })); //线程池则只需10秒左右
        }
        // 验证所有结果
        for (auto& f : futures)
        {
            f.get();
        }
        time_t nowDif = std::time(nullptr);
        std::cout << "\nThreadPool Test End : " << ctime(&nowDif) << std::endl;
        std::cout << "ThreadPool Test Duration : " << difftime(nowDif, now) << " seconds" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

    }
#endif
    std::cout << "Main Function End" << std::endl;
    return 0;
}