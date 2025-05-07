#include <condition_variable>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
class ThreadSynchronizationClass
{
public:
    struct ProductData
    {
        int id;
        std::string name;
    };

    struct queueDataInfo
    {
        std::queue<ProductData> queue;
        std::mutex mutex;
        std::condition_variable cv;
        std::string name;
    };
    ThreadSynchronizationClass();
    ~ThreadSynchronizationClass();

    void CreateProduct();
    void ProcessStage(queueDataInfo& A, queueDataInfo& B); // 生产者 - 消费者
    void ConsumeProduct();
    //测试生产者消费者模型
    void TestCreatorComsumerModel();

private:
    //产品多个处理阶段 A -> B ->C -> Data;
    queueDataInfo m_ProducerA;
    queueDataInfo m_ProducerAndConsumerB;
    queueDataInfo m_ProducerBndConsumerC;
    queueDataInfo m_ConsumerData;
    int m_id = 0;
    std::atomic<bool> m_stop{false};
    std::vector<std::thread> m_producers;
    std::vector<std::thread> m_consumers;
    std::vector<std::thread> m_producersToConsumers;

};