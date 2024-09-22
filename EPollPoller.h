#pragma once

#include "Poller.h" 

#include <vector>
#include <sys/epoll.h>
 

class EPollPoller : public Poller
{ 
public: 
    EPollPoller(EventLoop* loop);
    ~EPollPoller();
 
    TimeStamp poll(int timeoutMs, ChannelList *activeChannels) override;
    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:
    static const int kInitEventListSize = 16;

    // 查询 活跃的连接
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel *channel); // 更新 channel 事件

    using EventList = std::vector<epoll_event>;
    int epollfd_;  // epoll 核心事件表
    EventList events_;
};
