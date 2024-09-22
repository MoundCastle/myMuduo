#pragma once

#include "noncopyable.h"
#include "TimeStamp.h"

#include <functional>
#include <memory>

// channel :socket event hander

class EventLoop;

class Channel : noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp)>;
    Channel(EventLoop *loop, int fd);
    ~Channel();

    // 得到 epoll 通知，调用事件，通过回调函数。
    void handleEvent(TimeStamp receiveTime);

    void setReadCallback(ReadEventCallback cb)
    {
        readCallback_ = std::move(cb);
    }
    void setWriteCallback(EventCallback cb)
    {
        writeCallback_ = std::move(cb);
    }
    void setErrorCallback(EventCallback cb)
    {
        errorCallback_ = std::move(cb);
    }
    void setCloseCallback(EventCallback cb)
    {
        closeCallback_ = std::move(cb);
    }

    // 检测channel 是否仍旧有效
    void tie(const std::weak_ptr<void> &);

    // 查看 此 channel 状态
    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int recv) { revents_ = recv; }

    //  设置此 fd 监听事件
    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }
    void disableReading()
    {
        events_ &= ~kReadEvent;
        update();
    }

    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }
    void disableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }
    void disableAll()
    {
        events_ = kNoneEvent;
        update();
    }
    // 查看 fd 事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }  
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const { return events_ & kReadEvent; }

    int index() { return index_; }
    void set_index(int dx) { index_ = dx; }

    EventLoop *ownerLoop() { return loop_; }
    void remove();

private:
    void update();
    void handleEventWithGuard(TimeStamp receiveTime);

    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent; 

    EventLoop *loop_;
    const int fd_;
    int events_;  // 关注事件
    int revents_; //
    int index_;   // 状态

    std::weak_ptr<void> tie_; //
    bool tied_;

    //  各种回调函数， channel 负责监听 fd，所以得知所有的触发事件
    ReadEventCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;
};