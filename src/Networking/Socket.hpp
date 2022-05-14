
#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <stdexcept>

typedef unsigned short port_t;

namespace ws
{
    class Socket
    {
    public:
        Socket();
        Socket(int fd);
        ~Socket();

        void setFd(int fd);
        int getFd() const;

        void setup(const std::string &addr, port_t port);

        Socket accept();
        std::string recv(size_t size);
        void send(const std::string &data);
        
        void shutdown(int how);
        void close();

    protected:
        void bind(const std::string &addr, port_t port);
        void listen(int backlog);
        int _fd;
        struct sockaddr_in addr;
    };

} // namespace ws