#include "ThreadPool.h"

thread_local std::unique_ptr<ThreadPool::m_threadQueue> ThreadPool::m_localWorkQueue = nullptr;

void ThreadPool::WorkThread()
{
    m_localWorkQueue = std::make_unique<m_threadQueue>();
    while (!m_globalDone.load())
    {
        // 先从自己的队列中取任务
        if (m_localWorkQueue && !m_localWorkQueue->empty())
        {
            auto work = std::move(m_localWorkQueue->front());
            m_localWorkQueue->pop();
            work();
        }
        {
            std::lock_guard<std::mutex> lock(m_globalMutex);
            if (!m_workGlobalQueue.empty())
            {
                auto work = std::move(m_workGlobalQueue.front());
                m_workGlobalQueue.pop();
                work();
            }
            else
            {
                std::this_thread::yield(); // Yield to allow other threads to run
            }
        }
    }
}

ThreadPool::ThreadPool()
{
    m_globalDone = false;
    int threadCount = std::thread::hardware_concurrency();
 
    for (int i = 0; i < threadCount; ++i)
    {
        m_threads.emplace_back([this]()
        {
            WorkThread();
        });
    }
}

ThreadPool::~ThreadPool()
{
    m_globalDone.store(true);
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

