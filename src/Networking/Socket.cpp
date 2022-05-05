
#include "Socket.hpp"

namespace ws
{
    Socket::Socket()
    {
        _fd = -1;
    }

    Socket::Socket(int fd) : _fd(fd)
    {
    }

    Socket::~Socket()
    {
        if (_fd != -1)
            ::close(_fd);
    }

    void Socket::setFd(int fd)
    {
        _fd = fd;
    }

    int Socket::getFd() const
    {
        return _fd;
    }

    Socket Socket::accept()
    {
        int fd = ::accept(_fd, NULL, NULL);
        if (fd == -1)
            throw std::runtime_error("accept() failed");
        return Socket(fd);
    }

    void Socket::bind(const std::string &addr, port_t port)
    {
        struct sockaddr_in addr_in;
        addr_in.sin_family = AF_INET;
        addr_in.sin_port = htons(port);
        addr_in.sin_addr.s_addr = inet_addr(addr.c_str());
        if (::bind(_fd, (struct sockaddr *)&addr_in, sizeof(addr_in)) == -1)
            throw std::runtime_error("bind() failed");
    }

    void Socket::listen(int backlog)
    {
        if (::listen(_fd, backlog) == -1)
            throw std::runtime_error("listen() failed");
    }


    void Socket::send(const std::string &data)
    {
        if (::send(_fd, data.c_str(), data.size(), 0) == -1)
            throw std::runtime_error("send() failed");
    }

    std::string Socket::recv(size_t size)
    {
        char *buffer = new char[size];
        if (::recv(_fd, buffer, size, 0) == -1)
            throw std::runtime_error("recv() failed");
        std::string data(buffer);
        delete[] buffer;
        return data;
    }

    void Socket::shutdown(int how)
    {
        if (::shutdown(_fd, how) == -1)
            throw std::runtime_error("shutdown() failed");
    }

    void Socket::close()
    {
        if (_fd != -1)
        {
            ::close(_fd);
            _fd = -1;
        }
    }

} // namespace ws
