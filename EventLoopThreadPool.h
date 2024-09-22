#pragma once

#include "noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

#include <functional>
#include <memory>
#include <vector>
#include <string>

class EventLoop; 

class EventLoopThreadPool : noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseLoop, const std::string &nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }

    void start(const ThreadInitCallback &cb = ThreadInitCallback());

    EventLoop *getNextLoop();  // 多线程时使用轮询  

    std::vector<EventLoop *> getAllLoops();

    bool started() const
    {
        return started_;
    }

    const std::string &name() const
    {
        return name_;
    }

private:
    EventLoop *baseLoop_;
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;
};
