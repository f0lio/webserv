#pragma once

#include <sys/poll.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>

#include "Client.hpp"
#include "webserv.hpp"

#define MAX_FDS 1024

namespace ws
{
    class Configuration;
    class Cluster
    {
    public:
        Cluster(Configuration const& config);
        ~Cluster();

        void setup();
        void initPollFds();
        void run();

    private:
        std::set<int>       _server_fds;
        struct pollfd       _pollfds[MAX_FDS];
        int                 _nfds;
        bool                _running;
        bool                _setup;
        Configuration const& _config;
        std::map<int, VServer*> _fd_to_vserver;
        std::map<in_addr_t, std::vector<port_t> > _binded_listens;
    };

} // namespace ws