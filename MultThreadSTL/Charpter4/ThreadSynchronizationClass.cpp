#include "ThreadSynchronizationClass.h"

#include <string>

ThreadSynchronizationClass::ThreadSynchronizationClass()
{
}

ThreadSynchronizationClass::~ThreadSynchronizationClass()
{
}

#define MAX_ITEMS 1000
#define DelayLog 0
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
    //m_consumerCV.wait(lock, [this] { return !m_queueData.empty(); });
    auto waitResult = m_consumerCV.wait_for(lock, std::chrono::seconds(1), [this]
        {
            return !m_queueData.empty() || m_stop;
        });

    if (!waitResult)
    {
#if DelayLog
        // 超时判断
        std::cout << "超时：等待1秒未收到新产品" << std::endl;
#endif
        return;
    }
    if (m_queueData.empty())
    {
        return;
    }

    ProductData data = m_queueData.front();
    std::cout << "ProductData Info is name: " << data.name << " id is : " << data.id << std::endl;
    m_queueData.pop();
    if (data.id >= MAX_ITEMS)
    {
        m_stop = true;
        m_producerCV.notify_one();
    }
}

void ThreadSynchronizationClass::TestCreatorComsumerModel()
{
    std::thread creator([&]()
        {
            for (int i = 0; i < MAX_ITEMS; i++)
            {
                CreateProduct();
#if DelayLog
                std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
            }
        });
    std::thread consume([&]()
        {
            while (!m_stop)
            {
                ConsumeProduct();
            }
        });
    std::thread consume2([&]()
        {
            while (!m_stop)
            {
                ConsumeProduct();
            }
        });
    creator.join();
    consume.join();
    consume2.join();
}

void ThreadSynchronizationClass::TestFutureThreadUsed()
{
}

void ThreadSynchronizationClass::TestConditionVariableUsed()
{
}