#include "EventLoop.h"
#include "logger.h"
#include "Poller.h"
#include "Channel.h"

#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>

// 单例
__thread EventLoop *t_loopInThisThread = nullptr;

// 默认 接口超时时间
const int kPollTimeMs = 10000;

// 唤醒 
int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_FATAL("[%s:%d]: Failed in evenfd: %d n",__FILE__, __LINE__, errno);
    }
    return evtfd;
}
EventLoop::EventLoop()
    : looping_(false),
      quit_(false),
      callingPendingFunctors_(false),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(NULL)
{
    LOG_DEBUG("EventLoop created %p in thread %d \n ", this, threadId_);
    if (t_loopInThisThread)
    {
        LOG_FATAL("Another EventLoop %p exists in this thread: %d ", t_loopInThisThread, threadId_);
    }
    else
    {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
    wakeupChannel_->enableReading(); // 监听 epollin 读事件
}

EventLoop::~EventLoop()
{
    LOG_DEBUG("EventLoop %p of thread %d destructs in thread:%d \n ", this, threadId_, CurrentThread::tid());
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    looping_ = true;
    quit_ = false;
    LOG_DEBUG("EventLoop %p start looping", this);

    while (!quit_)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);

        for (Channel *channel : activeChannels_)
        {
            currentActiveChannel_ = channel;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = NULL;
        // 执行当前 eventloop 事件循环需要处理的回调工作 （主进程 给 subloop 注册回调。并唤醒使其执行 ）
        doPendingFunctors();
    }

    LOG_DEBUG("EventLoop %p stop looping", this);
    looping_ = false;
}

void EventLoop::quit()
{
    // 自己线程 调用 quit
    quit_ = true;
    // 其它线程 调用 quit ，需要唤醒该线程，使其退出，避免阻塞在 epoll_wait
    // 唤醒方式，给其 唤醒专用的 fd： wakeupFd_ 发一个信息
    // 唤醒后 会结束循环， 不然会等到下次接受 信号才 结束
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (const Functor &functor : functors)
    {
        functor();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor cb)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    //  在其它线程调用 ，需要唤醒loop 所在线程
    //  上次取出的回调函数正在执行， 这次插入的回调函数不会执行，会等到下次epoll_wait结束阻塞，才会执行
    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("EventLoop::handleRead() reads %lu bytes instead of 8", n);
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    return poller_->hasChannel(channel);
}

// 唤醒epoll所在线程，通过 向 wakeupfd_ 写一个数据（保留的唤醒通道）
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
    }
}