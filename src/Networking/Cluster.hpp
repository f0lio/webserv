#pragma once

#include "Client.hpp"
#include "webserv.hpp"
#include <sys/poll.h>


namespace ws
{
    class Configuration;
    class Cluster
    {
    public:
        Cluster(Configuration const &config);
        ~Cluster();

        void setup();
        void run();

    private:
        Configuration const &_config;
        // std::vector<Client> _clients;
        struct pollfd       _pollfds[1000];
        int                 _nfds;
    };

} // namespace ws