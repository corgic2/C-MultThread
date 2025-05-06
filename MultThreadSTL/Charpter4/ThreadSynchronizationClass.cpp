#include "ThreadSynchronizationClass.h"

#include <string>

ThreadSynchronizationClass::ThreadSynchronizationClass()
{
}

ThreadSynchronizationClass::~ThreadSynchronizationClass()
{
}

void ThreadSynchronizationClass::CreateProduct()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    ProductData data;
    data.id = ++m_id;
    data.name = std::to_string(m_id);
    m_queueData.push(data);
    m_consumerCV.notify_one();
}

void ThreadSynchronizationClass::ConsumeProduct()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto waitResult = m_consumerCV.wait_for(lock, std::chrono::seconds(1), [this]
    {
        return !m_queueData.empty() || m_stop.load(std::memory_order_relaxed);
    });

    if (!waitResult)
    {
        // 超时判断
        std::cout << "超时：等待1秒未收到新产品" << "current thread id is :" << std::this_thread::get_id() << std::endl;
        return;
    }
    if (m_queueData.empty())
    {
        return;
    }

    ProductData data = m_queueData.front();
    std::cout << "ProductData Info is name: " << data.name << " id is : " << data.id << std::endl;
    m_queueData.pop();
    m_producerCV.notify_one();
}

void ThreadSynchronizationClass::TestCreatorComsumerModel()
{
    for (int j = 0; j < 10; j++)
    {
        m_producers.emplace_back([&]()
        {
            while (!m_stop.load(std::memory_order_relaxed))
            {
                CreateProduct();
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }

        });
    }
    for (int j = 0; j < 100; j++)
    {
        m_consumers.emplace_back([&]()
        {
            while (!m_stop.load(std::memory_order_relaxed))
            {
                ConsumeProduct();
            }
        });
    }
    // 主线程等待 1 分钟
    std::this_thread::sleep_for(std::chrono::minutes(1));

    // 设置停止标志，通知所有线程退出
    m_stop.store(true, std::memory_order_relaxed);
    m_consumerCV.notify_all(); // 唤醒可能等待的消费者

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