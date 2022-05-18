#pragma once

#include "Client.hpp"
#include "webserv.hpp"
#include <sys/poll.h>
#include <algorithm>

#define MAX_FDS 1024

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
        std::map<in_addr_t, std::vector<port_t> > _binded_listens;
        std::map<int, VServer*> _fd_to_vserver;
        struct pollfd       _pollfds[MAX_FDS];
        int                 _nfds;
        bool                _running;
        bool                _setup;
    };

} // namespace ws