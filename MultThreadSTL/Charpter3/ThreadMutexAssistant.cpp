#include "ThreadMutexAssistant.h"

#include <functional>

ThreadMutexAssistant::ThreadMutexAssistant()
{
}

ThreadMutexAssistant::~ThreadMutexAssistant()
{
}

void ThreadMutexAssistant::LockGuardTestForThread(bool bJoin)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ++m_cnt;
    std::cout << "Cur Thread Cnt Value is :" << m_cnt << "  And the Thread is : " << std::this_thread::get_id() << "  Get The Thread JoinStatus is : " << (bJoin ? "join" : "detach") << std::endl;
}

void ThreadMutexAssistant::FunctionToLockGuardTestForThread()
{
    for (int i = 0; i < 100; i++)
    {
        std::function<void(bool)> function = std::bind(&ThreadMutexAssistant::LockGuardTestForThread, this, std::placeholders::_1);
        if (i % 5 != 0)
        {
            std::thread t(function, false);
            t.detach();
        }
        else
        {
            std::thread t(function, true);
            t.join();
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
}