

#pragma once

#include "webserv.hpp"
#include "./Socket.hpp"

class Configuration;

namespace ws
{
    class Server : protected Socket
    {
    public:
        Server(Configuration &config);
        ~Server();

        void start();
        void stop();

    private:
        Configuration *_config;
    };

} // namespace ws
