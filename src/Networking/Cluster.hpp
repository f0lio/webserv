#pragma once

#include <sys/poll.h>
#include <algorithm>
#include <string.h>
#include <fcntl.h>

#include "webserv.hpp"
#include "Client.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define MAX_FDS 10

namespace ws
{
    class Configuration;
    class Response;
    class Request;
    class Cluster
    {
    public:
        Cluster(Configuration const& config);
        ~Cluster();

        void setup();
        void run();

    private:
        
        std::set<int>       _server_fds;
        struct pollfd       _pollfds[MAX_FDS];
        int                 _nfds;
        bool                _running;
        bool                _setup;
        Configuration const& _config;
        // std::map<in_addr_t, std::vector<port_t> > _binded_listens;
        std::vector<struct Listen> _binded_listens;
        std::map<int, Request*> _fd_to_request;
        std::map<int, Response*> _fd_to_response;
        
        // each fd is associated to a vector of VServer, which are listening on the same address
        std::map<int, std::vector<VServer*> > _fd_to_vserver;   

        std::map<int, int> _client_to_server;

        // 
        int                 _client_fd; 
        struct sockaddr_in  _client_addr;
        socklen_t           _client_addr_len = sizeof(_client_addr);

        // 
        void initPollFds();
        void connectionHandler(int fd_index);
        void requestHandler(int fd_index);
        void responseHandler(int fd_index);
        bool isServerFd(int fd);
        VServer &getClientVServer(int client_fd);
    };

} // namespace ws


