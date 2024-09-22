#include "Thread.h"
#include "CurrentThread.h"

#include <semaphore.h>
#include <string>

std::atomic<int> Thread::numCreated_(0);

Thread::Thread(ThreadFunc func, const std::string &name)
    : started_(false), joined_(false), tid_(0), func_(std::move(func)), name_(name)
{
}
Thread::~Thread()
{
    if (started_ && !joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    started_ = true;
    sem_t sem;
    sem_init(&sem, false, 0);

    // 开启线程
    thread_ = std::shared_ptr<std::thread>(new std::thread([&]()
                                                           {
                                  tid_ = CurrentThread::tid(); // 获取线程的 tid
                                  sem_post(&sem);
                                  // 线程工作函数
                                  func_(); }));

    sem_wait(&sem); // 确保主线程退出 时
}

void Thread::join()
{
    joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++numCreated_;
    if (name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = buf;
    }
}