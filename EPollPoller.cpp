#include "EPollPoller.h" 
#include "logger.h" 
#include "Channel.h"

#include <string.h>
#include <unistd.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop *loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if (epollfd_ < 0)
    {
        LOG_FATAL("[%s:%s]: epoll_create error: %d",__FILE__, __func__, errno);
    }
}
EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

TimeStamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels)
{
    LOG_DEBUG("[%s:%d] %s: fd total count =%lu \n", __FILE__, __LINE__, __func__, channels_.size());
    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int savedErrno = errno;
    TimeStamp now(TimeStamp::now());
    if (numEvents > 0)
    {
        LOG_DEBUG("events happens \n");
        fillActiveChannels(numEvents, activeChannels);
        if (numEvents == events_.size()) //事件向量 满了
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_DEBUG("nothing happened \n");
    }
    else
    {
        // EINTR是Linux中系统调用被中断时的一种常见错误返回值
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_ERROR("EPollPoller::poll()");
        }
    }
    return now;
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{ 
    for (int i = 0; i < numEvents; ++i)
    {
        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::updateChannel(Channel *channel)
{
    const int index = channel->index();
    LOG_INFO("[%s:%d] %s: fd=%d events=%d index=%d \n", __FILE__, __LINE__, __func__, channel->fd(), channel->events(), index);
    if (index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if (index == kNew)
        {
            // 新增 channel
            channels_[fd] = channel;
        } // kdeleted

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // kadded   修改、删除操作
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel *channel)
{
    int fd = channel->fd();
    size_t n = channels_.erase(fd);

    int index = channel->index();
    LOG_INFO("[%s:%d] %s: fd=%d events=%d \n", __FILE__, __LINE__, __func__, channel->fd(), channel->events());

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::update(int operation, Channel *channel)
{
    struct epoll_event event;
    memset(&event, 0, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_INFO("[%s:%d] %s: fd=%d events=%d epoll_ctl op =%d \n", __FILE__, __LINE__, __func__, channel->fd(), channel->events(), operation);
    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("[%s:%d] %s: epoll_ctl_del error:%d \n", __FILE__, __LINE__, __func__, errno);
        }
        else
        {
            LOG_FATAL("[%s:%d] %s: epoll_ctl Fatal: %d \n", __FILE__, __LINE__, __func__, errno);
        }
    }
}