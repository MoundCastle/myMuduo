#include "TcpServer.h"
#include "logger.h"
#include "TcpConnection.h"

/*
      connectionCallback_( ),
      messageCallback_( ),
*/

static EventLoop *checkLoopNull(EventLoop *loop)
{
    if (loop == nullptr)
    {
        LOG_FATAL("[%s:%s:%d] loop is nullptr", __func__, __FILE__, __LINE__)
    }
    return loop;
}

TcpServer::TcpServer(EventLoop *loop,
                     const InetAddress &listenAddr,
                     const std::string &nameArg,
                     Option option)
    : loop_(checkLoopNull(loop)),
      ipPort_(listenAddr.toIpPort()),
      name_(nameArg),
      acceptor_(new Acceptor(loop, listenAddr, option == kReusePort)),
      threadPool_(new EventLoopThreadPool(loop, name_)),
      connectionCallback_(),
      messageCallback_(),
      nextConnId_(1),
      started_(0)
{
    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
    LOG_DEBUG("TcpServer::~TcpServer [%s] destructing", name_.c_str());

    for (auto &item : connections_)
    {
        TcpConnectionPtr conn(item.second);
        item.second.reset();
        conn->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, conn)); // 销毁连接
    }
}

void TcpServer::setThreadNum(int numThreads)
{
    if (numThreads < 0)
    {
        LOG_ERROR("[%s:%d] numThreads < 0 ,set numThreads = default (3)", __FILE__, __LINE__);
        numThreads = 3;
    }
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
    if (started_++ == 0)
    {
        threadPool_->start(threadInitCallback_);

        loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

// 传给 acceptor 处理新连接 的回调
void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
    EventLoop *ioLoop = threadPool_->getNextLoop();
    char buf[64];
    snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    LOG_INFO("TcpServer::newConnection [%s] - new connection [%s] from %s ", connName.c_str(), name_.c_str(), peerAddr.toIpPort().c_str());
    // 通过sockfd 获取 其绑定的本机 ip地址和端口信息
    sockaddr_in localAddress;
    bzero(&localAddress, sizeof localAddress);
    socklen_t addrlen = static_cast<socklen_t>(sizeof localAddress);
    if (::getsockname(sockfd, (sockaddr *)&localAddress, &addrlen) < 0)
    {
        LOG_ERROR("sockets::getlocalAddress");
    }

    InetAddress localAddr(localAddress);

    // TCPconnection  连接对象
    TcpConnectionPtr conn(new TcpConnection(ioLoop,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));

    connections_[connName] = conn;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    // 设置如何关闭连接的回调
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe

    ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn)
{
    // FIXME: unsafe
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn)
{
    LOG_INFO("TcpServer::removeConnectionInLoop [%s] - connection %s  \n", name_.c_str(), conn->name().c_str());
    connections_.erase(conn->name());

    EventLoop *ioLoop = conn->getLoop();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}