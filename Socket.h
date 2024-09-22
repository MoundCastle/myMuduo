#pragma once

#include "noncopyable.h"

class InetAddress;

class Socket
{
public:
    explicit Socket(int sockfd) : sockfd_(sockfd) {};
    ~Socket();

    // 返回 文件描述符
    int fd() const { return sockfd_; }
    void bindAddress(const InetAddress & localaddr);
    void listen() ;
    int accept(InetAddress * peeraddr);

    // 关闭写端
    void shutdownWrite();

    void setTcpNoDelay(bool on);
    void setReuseAddr(bool on);
    void setReusePort(bool on);
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};
