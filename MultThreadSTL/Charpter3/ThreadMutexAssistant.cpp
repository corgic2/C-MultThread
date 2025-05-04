#include "ThreadMutexAssistant.h"

#include <algorithm>
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
    //析构自动解锁
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

#define DeadLockWatch 0 // 观察死锁现象则为1，否则为0
void ThreadMutexAssistant::DeadLockTest()
{
    std::thread t1(&ThreadMutexAssistant::DeadLockTest1, this);
    std::thread t2(&ThreadMutexAssistant::DeadLockTest2, this);
    t1.join();
    t2.join();
    //此时还可以观察到，std::endl是等待两个Lockmutex输出完毕后再进行输出
    //也就是在线程运行时，<<输入流需要额外时间处理，导致输出完第一个字符串时，第二个输出流还未开始
    //导致Lock mutex 与 Lock mutex2连续输出而没有进行换行(输出完毕后等待输入流处理)

    //而使用unique_lock 与 std::lock 时，则不会出现由于输出流处理时间问题导致输出异常
}

void ThreadMutexAssistant::DeadLockTest1()
{
#if DeadLockWatch
    m_mutex.lock();
    std::cout << "Lock mutex" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    m_mutex2.lock();
    std::cout << "DeadLockTest1 Used" << std::endl;
    m_mutex2.unlock();
    m_mutex.unlock();
#else
    std::unique_lock<std::mutex> lock1(m_mutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(m_mutex2, std::defer_lock);
    std::lock(lock1, lock2);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "DeadLockTest1 Used" << std::endl;
#endif
}

void ThreadMutexAssistant::DeadLockTest2()
{
#if DeadLockWatch
    m_mutex2.lock();
    std::cout << "Lock mutex2" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    m_mutex.lock();
    std::cout << "DeadLockTest2 Used" << std::endl;
    m_mutex.unlock();
    m_mutex2.unlock();
#else
    std::unique_lock<std::mutex> lock1(m_mutex, std::defer_lock);
    std::unique_lock<std::mutex> lock2(m_mutex2, std::defer_lock);
    std::lock(lock1, lock2);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "DeadLockTest2 Used" << std::endl;
#endif
}