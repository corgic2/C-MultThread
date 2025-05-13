#include "ThreadPool.h"
thread_local int ThreadPool::m_threadIndex = 0;


void ThreadPool::WorkThread()
{
    while (!m_globalDone.load())
    {
        bool bflag = false;
        // 先从自己的队列中取任务
        {
            std::lock_guard<std::mutex> localMutex(m_localWorkQueue[m_threadIndex].mutex);
            if (m_localWorkQueue[m_threadIndex].queue && !m_localWorkQueue[m_threadIndex].queue->empty())
            {
                auto work = std::move(m_localWorkQueue[m_threadIndex].queue->front());
                m_localWorkQueue[m_threadIndex].queue->pop();
                //std::cout << "Thread ID: " << std::this_thread::get_id() << " 取到任务" << std::endl;
                work();
                bflag = true; // 标记为已执行任务
            }
        }
        //未取到任务时，再从其他队列中取任务
        if (!bflag)
        {
            int stealIndex = (m_threadIndex + 1) % m_localWorkQueue.size();
            for (int i = stealIndex; i < m_localWorkQueue.size();)
            {
                if (i == m_threadIndex)
                {
                    break; // 不偷自己的队列
                }
                std::unique_lock<std::mutex> otherMutex(m_localWorkQueue[i].mutex, std::try_to_lock);
                if (otherMutex.owns_lock() && m_localWorkQueue[i].queue && !m_localWorkQueue[i].queue->empty())
                {
                    auto work = std::move(m_localWorkQueue[i].queue->front());
                    m_localWorkQueue[i].queue->pop();
                    otherMutex.unlock(); // 释放锁
                    work();
                    //std::cout << "Thread ID: " << std::this_thread::get_id() << " 取到任务" << std::endl;
                    bflag = true;
                    break;
                }
                i = (i + 1) % m_localWorkQueue.size(); // 轮询下一个队列
            }
        }
        //如果都没有任务，则从全局队列中取任务
        if (!bflag)
        {
            std::unique_lock<std::mutex> lock(m_globalMutex);
            if (!m_workGlobalQueue.empty())
            {
                auto work = std::move(m_workGlobalQueue.front());
                m_workGlobalQueue.pop();
                work();
                //std::cout << "Thread ID: " << std::this_thread::get_id() << " 取到任务" << std::endl;
            }
        }
    }
}

int ThreadPool::FindLightestQueue()
{
    int min_size = std::numeric_limits<size_t>::max();
    int target_idx = 0;
    for (size_t i = 0; i < m_localWorkQueue.size(); ++i)
    {
        std::lock_guard<std::mutex> lock(m_localWorkQueue[i].mutex);
        if (m_localWorkQueue[i].queue->size() < min_size)
        {
            min_size = m_localWorkQueue[i].queue->size();
            target_idx = i;
        }
    }
    return target_idx;
}

ThreadPool::ThreadPool()
{
    m_globalDone = false;
    int threadCount = std::thread::hardware_concurrency();
 
    for (int i = 0; i < threadCount; ++i)
    {
        m_threads.emplace_back([this, i]()
        {
            m_threadIndex = i;
            WorkThread();
        });
        ST_localThreadQueue localQueue;
        localQueue.queue = std::make_unique<m_threadQueue>();
        m_localWorkQueue.emplace_back(std::move(localQueue));
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

