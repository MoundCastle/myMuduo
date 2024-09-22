#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

class InetAddress
{
public:
    explicit InetAddress(uint16_t port = 6000, std::string ip = "127.0.0.1");
    explicit InetAddress(const sockaddr_in &addr) : addr_(addr) {}

    uint16_t toPort() const;
    std::string toIp() const;
    std::string toIpPort() const;

    const sockaddr_in *getSocket() const
    {
        return &addr_;
    }
    void setSocket(const sockaddr_in &addr)
    {
        addr_ = addr;
    }

    ~InetAddress() = default;

private:
    sockaddr_in addr_;
};
