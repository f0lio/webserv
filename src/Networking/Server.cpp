

#include "./Server.hpp"

namespace ws
{
    // Server::Server(parser::Context &serverConfig)
    Server::Server()
    {
        _fd = -1;
        // prepare(serverConfig);
        check_config();
    }

    Server::~Server()
    {
        if (_fd != -1)
            ::close(_fd);
    }

    void Server::start()
    {
        if (_fd != -1)
            return;

        // _fd = ::socket(AF_INET, SOCK_STREAM, 0);
        // if (_fd == -1)
        //     throw std::runtime_error("socket() failed");

        // struct sockaddr_in addr;
        // addr.sin_family = AF_INET;
        // addr.sin_port = htons(_config->port());
        // addr.sin_addr.s_addr = INADDR_ANY;
        // if (::bind(_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        //     throw std::runtime_error("bind() failed");

        // if (::listen(_fd, _config->max_connections()) == -1)
        //     throw std::runtime_error("listen() failed");
    }

    void Server::stop()
    {
        if (_fd == -1)
            return;

        ::close(_fd);
        _fd = -1;
    }

    // get required data from Configuration and set them in a map<str, str> for easy access

    // void Server::prepare(parser::Context &serverConfig)
    void Server::prepare()
    {

    }

    void Server::check_config() const
    {

    }

} // namespace ws
