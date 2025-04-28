#include <iostream>
#include <thread>
#include <mutex>
class ThreadMutexAssistant
{
public:
    ThreadMutexAssistant();
    ~ThreadMutexAssistant();
    void LockGuardTestForThread(bool bJoin);
    void FunctionToLockGuardTestForThread();
private:
    int m_cnt = 10;
    std::mutex m_mutex;
};
