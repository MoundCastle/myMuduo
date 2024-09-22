#pragma once

// IO 复用
#include "noncopyable.h"
#include "TimeStamp.h"

#include <vector>
#include <unordered_map>

class Channel;
class EventLoop;

class Poller : noncopyable
{ 
public: 
    using ChannelList = std::vector<Channel *>;
    Poller(EventLoop *loop);
    virtual ~Poller();

    // 纯虚函数， 设置调用接口
    virtual TimeStamp poll(int timeoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;
    
    // 是否在此POLLER
    virtual bool hasChannel(Channel *channel) const;

    // 获取默认的 Pooler 
    static Poller* newDefaultPoller(EventLoop* loop);  
protected:
    using ChannelMap = std::unordered_map<int, Channel* >;
    ChannelMap channels_;  // 存放注册过的 channel

private:
    EventLoop* ownerLoop_;
};
 
