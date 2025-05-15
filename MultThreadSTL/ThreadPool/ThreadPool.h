#pragma once

#include <functional>
#include <future>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
class FunctionOnlyMove
{
private:
    struct ST_FuncBase
    {
        //基类函数
        virtual void call()
        {
        };

        virtual ~ST_FuncBase()
        {
        }
    };

    template <typename F>
    struct ST_FuncImpl : public ST_FuncBase
    {
        ST_FuncImpl(F&& func)
            : m_func(std::forward<F>(func))
        {
        }

        void call() override
        {
            m_func();
        }

        F m_func;
    };

public:
    template <typename F>
    FunctionOnlyMove(F&& func)
        : m_funcImpl(new ST_FuncImpl<F>(std::forward<F>(func)))
    {
    }

    FunctionOnlyMove() = default;
    //三种拷贝删除
    FunctionOnlyMove(const FunctionOnlyMove&) = delete;
    FunctionOnlyMove& operator=(const FunctionOnlyMove&) = delete;
    FunctionOnlyMove(FunctionOnlyMove& other) = delete;

    //移动构造 
    FunctionOnlyMove(FunctionOnlyMove&& other)
        : m_funcImpl(std::move(other.m_funcImpl))
    {
        other.m_funcImpl = nullptr;
    }

    FunctionOnlyMove& operator=(FunctionOnlyMove&& other)
    {
        if (this != &other)
        {
            m_funcImpl = std::move(other.m_funcImpl);
            other.m_funcImpl = nullptr;
        }
        return *this;
    }

    void operator()()
    {
        if (m_funcImpl)
        {
            m_funcImpl->call();
        }
    }

    std::unique_ptr<ST_FuncBase> m_funcImpl;
};

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

public:
    using m_threadQueue = std::queue<FunctionOnlyMove>;

    struct ST_localThreadQueue
    {
        ST_localThreadQueue() = default;

        ST_localThreadQueue(ST_localThreadQueue&& other)
            : queue(std::move(other.queue))
        {
        }

        ST_localThreadQueue& operator=(ST_localThreadQueue&& other)
        {
            if (this != &other)
            {
                queue = std::move(other.queue);
            }
            return *this;
        }

        ST_localThreadQueue& operator=(const ST_localThreadQueue& other) = delete;
        ST_localThreadQueue(const ST_localThreadQueue& other) = delete;
        ST_localThreadQueue(ST_localThreadQueue& other) = delete;

        std::unique_ptr<m_threadQueue> queue;
        std::mutex mutex;
    };
    template <typename Func>
    std::future<typename std::result_of<Func()>::type> AddTask(Func f)
    {
        using result = typename std::result_of<Func()>::type;
        std::packaged_task<result()> task(std::move(f));
        //task不可复制，只能通过移动,因此，task内的函数也应当仅支持移动
        //并且要获取函数返回值，则同步需要模板函数编程，因此自定义函数类
        std::future<result> res = task.get_future();
        //每次寻找最轻松的队列，以防任务不均衡
        size_t idx = FindLightestQueue();
        std::unique_lock<std::mutex> lock(m_localWorkQueue[idx].mutex, std::try_to_lock);
        if (lock.owns_lock() && m_localWorkQueue[idx].queue)
        {
            m_localWorkQueue[idx].queue->emplace(std::move(task));
            return res;
        }
        // 回退到全局队列
        std::lock_guard<std::mutex> globalLock(m_globalMutex);
        m_workGlobalQueue.emplace(std::move(task));
        return res;
    }
private:
    void WorkThread();
    int FindLightestQueue();
private:
    std::atomic_bool m_globalDone;
    std::queue<FunctionOnlyMove> m_workGlobalQueue;
    std::mutex m_globalMutex;

    std::vector<ST_localThreadQueue> m_localWorkQueue; //根据当前线程不同各自拥有一个任务队列，减少全局队列的锁竞争
    static thread_local int m_threadIndex;

    std::atomic<size_t> m_nextQueueIndex{0}; // 新增原子计数器
    std::vector<std::thread> m_threads;
};
