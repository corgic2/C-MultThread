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
    //m_mutex.lock();
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    ++m_cnt;
    //m_mutex.unlock();
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
    //等待剩余的输出完毕
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void ThreadMutexAssistant::UniqueLockTestForThread(bool bJoin)
{
    //在lock前等待与lock后等待是不同的,lock前等待可能多个线程同时等待
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    std::unique_lock<std::mutex> lock(m_mutex);
    //lock后等待则每次只会等待一个线程
    //std::this_thread::sleep_for(std::chrono::seconds(1));
    m_cnt += 10;
    //lock.unlock(); 去掉sleep时，并且unlock在cout前时，会出现一个线程输出一半，而另一个线程抢占io进行输出，会出现乱序输出
    std::cout << "\nCur Thread Cnt Value is :" << m_cnt << "  And the Thread is : " << std::this_thread::get_id() << "  Get The Thread JoinStatus is : " << (bJoin ? "join" : "detach") << std::endl;
    lock.unlock();
}

void ThreadMutexAssistant::FunctionToUniqueLockTestForThread()
{
    for (int i = 0; i < 100; i++)
    {
        std::function<void(bool)> function = std::bind(&ThreadMutexAssistant::UniqueLockTestForThread, this, std::placeholders::_1);
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
    //等待剩余的输出完毕
    std::this_thread::sleep_for(std::chrono::seconds(3));
}