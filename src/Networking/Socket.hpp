
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

        void set_fd(int fd);
        int get_fd() const;

        void bind(const std::string &addr, port_t port);
        void connect(const std::string &addr, port_t port);
        Socket accept();
        void listen(int backlog);
        void send(const std::string &data);
        std::string recv(size_t size);
        void shutdown(int how);

        void close();

    protected:
        int _fd;
    };

} // namespace ws