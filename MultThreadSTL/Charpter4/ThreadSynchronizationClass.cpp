#include "ThreadSynchronizationClass.h"

#include <string>

ThreadSynchronizationClass::ThreadSynchronizationClass()
{
}

ThreadSynchronizationClass::~ThreadSynchronizationClass()
{
}

#define MAX_ITEMS 1000
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
    m_consumerCV.wait(lock, [this] { return !m_queueData.empty(); });

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
            }
        });
    std::thread consume([&]()
        {
            while (!m_stop)
            {
                ConsumeProduct();
            }
        });
    creator.join();
    consume.join();
}

void ThreadSynchronizationClass::TestFutureThreadUsed()
{
}

void ThreadSynchronizationClass::TestConditionVariableUsed()
{
}