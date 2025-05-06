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
    ThreadSynchronizationClass();
    ~ThreadSynchronizationClass();

    void CreateProduct();  // 生产者
    void ConsumeProduct(); // 消费者
    //测试没有条件变量的生产者消费者
    void TestCreatorComsumerModel();
    //测试条件变量
    void TestConditionVariableUsed();
    //测试Future
    void TestFutureThreadUsed();

private:
    std::queue<ProductData> m_queueData;
    int m_id = 0;
    std::mutex m_mutex;
    std::atomic<bool> m_stop{false};
    std::condition_variable m_consumerCV;
    std::condition_variable m_producerCV;
    std::vector<std::thread> m_producers;
    std::vector<std::thread> m_consumers;
};