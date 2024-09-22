#include "Channel.h"
#include "EventLoop.h"
#include "logger.h"

#include <sys/epoll.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false)
{
}

Channel::~Channel()
{
}
void Channel::tie(const std::weak_ptr<void> &obj)
{
    tie_ = obj;
    tied_ = true;
}

// 修改 此 fd 关注的 events 后更新 poller 中 fd 对应事件的 epoll——ctl；
// EventLoop 包含：ChannelList  poller ；
void Channel::update()
{
    // addcode
    loop_->updateChannel(this);
}
void Channel::remove()
{
    // addcode
    loop_->removeChannel(this);
}

void Channel::handleEvent(TimeStamp receiveTime)
{
    std::shared_ptr<void> guard;
    if (tied_)
    {
        guard = tie_.lock();
        if (guard)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void Channel::handleEventWithGuard(TimeStamp receiveTime)
{
    LOG_INFO("[%s:%d] %s: channel handleEvents revents: %d \n", __FILE__, __LINE__, __func__, revents_);
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if (closeCallback_)
        {
            closeCallback_();
        }
    }
    if (revents_ & EPOLLERR)
    {
        if (errorCallback_)
        {
            errorCallback_();
        }
    }
    if (revents_ & EPOLLIN)
    {
        if (readCallback_)
        {
            readCallback_(receiveTime);
        }
    }
    if (revents_ & (EPOLLOUT | EPOLLPRI)) // EPOLLPRI 有紧急的数据可读
    {
        if (writeCallback_)
        {
            writeCallback_();
        }
    }
}
