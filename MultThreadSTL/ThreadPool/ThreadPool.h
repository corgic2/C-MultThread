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
    template <typename Func>
    std::future<typename std::result_of<Func()>::type> AddTask(Func f)
    {
        using result = typename std::result_of<Func()>::type;
        std::packaged_task<result()> task(std::move(f));
        //task不可复制，只能通过移动,因此，task内的函数也应当仅支持移动
        //并且要获取函数返回值，则同步需要模板函数编程，因此自定义函数类
        std::future<result> res = task.get_future();
        if (m_localWorkQueue)
        {
            //如果当前线程拥有自己的任务队列，则直接将任务放入自己的队列中
            m_localWorkQueue->emplace(std::move(task));
        }
        else
        {
            //如果当前线程没有自己的任务队列，则将任务放入全局队列中
            std::lock_guard<std::mutex> lock(m_globalMutex);
            m_workGlobalQueue.emplace(std::move(task));
        }
        return res;
    }
private:
    void WorkThread();
    
private:
    std::atomic_bool m_globalDone;
    std::queue<FunctionOnlyMove> m_workGlobalQueue;
    std::mutex m_globalMutex;
    using m_threadQueue = std::queue<FunctionOnlyMove>;
    static thread_local std::unique_ptr<m_threadQueue> m_localWorkQueue; //根据当前线程不同各自拥有一个任务队列，减少全局队列的锁竞争
    std::vector<std::thread> m_threads;
};
