#pragma once

#include "noncopyable.h"
#include "TimeStamp.h"
#include "CurrentThread.h"

#include <atomic>
#include <functional>
#include <vector>
#include <memory>
#include <mutex>

class Channel;
class Poller;
class TimeStamp;

// epoll
class EventLoop : public noncopyable
{
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();
    void loop(); // 开启事件循环
    void quit();

    TimeStamp pollReturnTime() const { return pollReturnTime_; }

    void runInLoop(Functor cb);   // 在当前事件循环执行 cb
    void queueInLoop(Functor cb); // cb 放入队列，唤醒loop 所在线程，执行 cb

    void wakeup();
    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:
    using ChannelList = std::vector<Channel *>; 

    void handleRead(); // waked up
    void doPendingFunctors();

    std::atomic<bool> looping_;  
    std::atomic<bool> quit_;

    const pid_t threadId_; // 创建此 Eventloop 的线程ID
    TimeStamp pollReturnTime_;
    std::unique_ptr<Poller> poller_;

    int wakeupFd_; // 唤醒线程用
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    Channel *currentActiveChannel_;

    std::atomic<bool> callingPendingFunctors_;
    std::mutex mutex_;
    std::vector<Functor> pendingFunctors_; // 存储 loop 需要执行的 所有 回调操作
};  
