#include "ThreadPool.h"

void ThreadPool::WorkThread()
{
    while (!m_done.load())
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_workQueue.empty())
        {
            auto work = std::move(m_workQueue.front());
            m_workQueue.pop();
            work();
        }
        else
        {
            std::this_thread::yield(); // Yield to allow other threads to run
        }
    }
}

ThreadPool::ThreadPool()
{
    m_done = false;
    int threadCount = std::thread::hardware_concurrency();
    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([this]()
        {
            WorkThread();
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_done.store(true);
    for (auto& thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

