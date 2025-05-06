#include "ThreadSynchronizationClass.h"

#include <string>

ThreadSynchronizationClass::ThreadSynchronizationClass()
{
}

ThreadSynchronizationClass::~ThreadSynchronizationClass()
{
}

const int maxItem = 100;

//队列满时，生产者阻塞，队列空时，消费者等待
void ThreadSynchronizationClass::CreateProduct()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    m_producerCV.wait(lock, [this]()
    {
        return m_queueData.size() < maxItem || m_stop.load();
    });

    if (m_stop.load())
    {
        return;
    }
    ProductData data;
    data.id = ++m_id;
    data.name = std::to_string(m_id);
    m_queueData.push(data);
    m_consumerCV.notify_one();
}

void ThreadSynchronizationClass::ConsumeProduct()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto waitResult = m_consumerCV.wait_for(lock, std::chrono::seconds(10), [this]
    {
        return !m_queueData.empty() || m_stop.load(std::memory_order_relaxed);
    });

    if (!waitResult)
    {
        if (m_stop.load())
        {
            return; // 优先检查停止标志
        }
        // 超时判断
        std::cout << "超时：等待10秒未收到新产品" << "current thread id is :" << std::this_thread::get_id() << std::endl;
        return;
    }
    if (m_queueData.empty())
    {
        return;
    }

    ProductData data = m_queueData.front();
    std::cout << "ProductData Info is name: " << data.name << " id is : " << data.id << "current QueueSize is : " << m_queueData.size() << std::endl;
    m_queueData.pop();
    m_producerCV.notify_one();
}

void ThreadSynchronizationClass::TestCreatorComsumerModel()
{
    for (int j = 0; j < 100; j++)
    {
        m_producers.emplace_back([&]()
        {
            while (!m_stop.load(std::memory_order_acquire))
            {
                CreateProduct();
            }
        });
    }
    for (int j = 0; j < 10; j++)
    {
        m_consumers.emplace_back([&]()
        {
            while (!m_stop.load(std::memory_order_acquire))
            {
                ConsumeProduct();
                std::this_thread::sleep_for(std::chrono::seconds(2));
            }
        });
    }
    // 主线程等待 1 分钟
    std::this_thread::sleep_for(std::chrono::minutes(1));

    // 设置停止标志，通知所有线程退出
    m_stop.store(true, std::memory_order_release);
    m_consumerCV.notify_all(); // 唤醒可能等待的消费者
    m_producerCV.notify_all(); // 必须同时唤醒生产者

    // 等待所有线程结束
    for (auto& producer : m_producers)
    {
        if (producer.joinable())
        {
            producer.join();
        }
    }
    for (auto& consumer : m_consumers)
    {
        if (consumer.joinable())
        {
            consumer.join();
        }
    }
}

void ThreadSynchronizationClass::TestFutureThreadUsed()
{
}

void ThreadSynchronizationClass::TestConditionVariableUsed()
{
}
