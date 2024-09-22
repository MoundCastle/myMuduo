#include "Poller.h"
#include "EPollPoller.h" 

#include <stdlib.h>
 
Poller *Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return nullptr; // 没实现 poll  只有 epoll
    }
    else
    {
        // return nullptr;
        return new EPollPoller(loop);
    }
}
