#include <condition_variable>
#include <future>
#include <iostream>
#include <thread>
class ThreadSynchronizationClass
{
public:
    struct ProductData
    {
        int id;
        std::string data;
    };
    ThreadSynchronizationClass();
    ~ThreadSynchronizationClass();

    void CreateProduct();  // 生产者
    void ConsumeProduct(); // 消费者
    //测试条件变量
    void TestConditionVariableUsed();
    //测试Future
    void TestFutureThreadUsed();

private:
    std::vector<ProductData> m_vectorData;
    std::mutex m_mutexCnt;
};