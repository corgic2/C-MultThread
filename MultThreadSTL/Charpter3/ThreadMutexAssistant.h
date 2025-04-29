#include <iostream>
#include <thread>
#include <mutex>
class ThreadMutexAssistant
{
public:
    ThreadMutexAssistant();
    ~ThreadMutexAssistant();
    //测试LockGuard的RAII机制
    void LockGuardTestForThread(bool bJoin);
    void FunctionToLockGuardTestForThread();
    //测试UniqueLock的灵活加锁解锁
    void UniqueLockTestForThread(bool bJoin);
    void FunctionToUniqueLockTestForThread();
private:
    int m_cnt = 10;
    std::mutex m_mutex;
};
