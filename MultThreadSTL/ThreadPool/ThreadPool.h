#pragma once

#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class ThreadPool
{
public:
    // Constructor and Destructor
    ThreadPool();
    ~ThreadPool();

public:
    template <typename Func>
    void AddTask(Func f)
    {
        m_workQueue.push(std::function<void()>(f));
    }
private:
    void WorkThread();
    
private:
    std::atomic_bool m_done;
    std::queue<std::function<void()>> m_workQueue;
    std::mutex m_mutex;
    std::vector<std::thread> threads;
};
