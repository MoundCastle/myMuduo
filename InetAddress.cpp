#include "InetAddress.h"

#include <iostream>
#include <cstring>

InetAddress::InetAddress(uint16_t port, std::string ip)
{
    memset(&addr_, 0, sizeof addr_);
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
}
uint16_t InetAddress::toPort() const
{
    return ntohs(addr_.sin_port);
}
std::string InetAddress::toIp() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, 64);
    return buf;
}

// IP:Port
std::string InetAddress::toIpPort() const
{
    char buf[64] = {0};
    inet_ntop(AF_INET, &addr_.sin_addr, buf, 64);
    size_t buflen = strlen(buf);
    sprintf(buf + buflen, ":%u", ntohs(addr_.sin_port));
    return buf;
}

// int main()
// {
//     InetAddress inet(200, "127.0.2.3");

//     std::cout << inet.toIp() << std::endl;
//     std::cout << inet.toPort() << std::endl;
//     std::cout << inet.toIpPort() << std::endl;

//     return 0;
// }