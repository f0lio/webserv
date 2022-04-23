

#pragma once

#include "webserv.hpp"
#include "./Socket.hpp"

class Configuration;

namespace ws
{
    class Server : protected Socket
    {
    public:
        // Server(parser::Context &serverConfig);
        Server();
        ~Server();

        void start();
        void stop();

    private:
        void check_config() const;
        
        void prepare();
        // void prepare(parser::Context &serverConfig);
        std::map<std::string, std::string> _config;
    };

} // namespace ws
