#include "ThreadSynchronizationClass.h"

#include <string>

ThreadSynchronizationClass::ThreadSynchronizationClass()
{
    m_ProducerA.name = "A";
    m_ProducerAndConsumerB.name = "B";
    m_ProducerBndConsumerC.name = "C";
    m_ConsumerData.name = "Data";

}

ThreadSynchronizationClass::~ThreadSynchronizationClass()
{
}

const int maxItem = 100;

void ThreadSynchronizationClass::CreateProduct()
{
    std::unique_lock<std::mutex> lock(m_ProducerA.mutex);

    m_ProducerA.cv.wait(lock, [this]()
    {
        return m_ProducerA.queue.size() < maxItem || m_stop.load();
    });

    if (m_stop.load())
    {
        return;
    }

    ProductData data;
    data.id = ++m_id;
    data.name = std::to_string(m_id);
    m_ProducerA.queue.push(data);
    m_ProducerA.cv.notify_one();
}
//队列满时，生产者阻塞，队列空时，消费者等待
void ThreadSynchronizationClass::ProcessStage(queueDataInfo& dataA, queueDataInfo& dataB)
{
    const int batchItems = 10;
    std::vector<ProductData> batch;
    std::unique_lock<std::mutex> lock(dataA.mutex);
    dataA.cv.wait(lock, [&]()
    {
        return dataA.queue.size() >= batchItems || m_stop.load();
    });

    if (m_stop.load())
    {
        return;
    }
    for (int i = 0; i < batchItems; i++)
    {
        ProductData data = dataA.queue.front();
        dataA.queue.pop();
        batch.push_back(data);
    }

    lock.unlock();
    dataA.cv.notify_all(); //被取出后应该及时提醒补货，否则容易造成生产端一直等待

    std::unique_lock<std::mutex> consumerLock(dataB.mutex);

    dataB.cv.wait(consumerLock, [&]
    {
        return (dataB.queue.size() + batchItems) <= maxItem || m_stop.load();
    });

    if (m_stop.load())
    {
        return;
    }
    for (auto& tmp : batch)
    {
        dataB.queue.push(tmp);
    }
    consumerLock.unlock();
    dataB.cv.notify_all(); // 加工完后立刻通知下游队列取货，不然无法进一步加工
}

void ThreadSynchronizationClass::ConsumeProduct()
{
    std::unique_lock<std::mutex> lock(m_ConsumerData.mutex);
    m_ConsumerData.cv.wait(lock, [this]
    {
        return !m_ConsumerData.queue.empty() || m_stop.load(std::memory_order_relaxed);
    });

    ProductData data = m_ConsumerData.queue.front();
    std::cout << "ProductData Info is name: " << data.name << " id is : " << data.id << "  current QueueSize is : " << m_ConsumerData.queue.size() << std::endl;
    m_ConsumerData.queue.pop();
    lock.unlock();
    m_ConsumerData.cv.notify_one();

    //通知C
    m_ProducerBndConsumerC.cv.notify_one();
}

void ThreadSynchronizationClass::TestCreatorComsumerModel()
{
    for (int i = 0; i < 10; i++)
    {
        m_producers.emplace_back([&]()
        {
            while (!m_stop.load())
            {
                CreateProduct();
            }
        });
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            m_producersToConsumers.emplace_back([&,i]()
            {
                while (!m_stop.load(std::memory_order_acquire))
                {
                    switch (i)
                    {
                        case 0:
                            ProcessStage(m_ProducerA, m_ProducerAndConsumerB);
                            break;
                        case 1:
                            ProcessStage(m_ProducerAndConsumerB, m_ProducerBndConsumerC);
                            break;
                        case 2:
                            ProcessStage(m_ProducerBndConsumerC, m_ConsumerData);
                            break;
                    }
                }
            });
        }
    }

    for (int j = 0; j < 10; j++)
    {
        m_consumers.emplace_back([&]()
        {
            while (!m_stop.load(std::memory_order_acquire))
            {
                ConsumeProduct();
            }
        });
    }
    // 主线程等待 1 分钟
    std::this_thread::sleep_for(std::chrono::minutes(10));

    // 设置停止标志，通知所有线程退出
    m_stop.store(true, std::memory_order_seq_cst);
    m_ProducerA.cv.notify_all();
    m_ProducerAndConsumerB.cv.notify_all();
    m_ProducerBndConsumerC.cv.notify_all();
    m_ConsumerData.cv.notify_all();

    // 等待所有线程结束
    for (auto& producer : m_producers)
    {
        if (producer.joinable())
        {
            producer.join();
        }
    }
    for (auto& producerToConsumer : m_producersToConsumers)
    {
        if (producerToConsumer.joinable())
        {
            producerToConsumer.join();
        }
    }
    // 等待所有线程结束
    for (auto& consumer : m_consumers)
    {
        if (consumer.joinable())
        {
            consumer.join();
        }
    }
}
