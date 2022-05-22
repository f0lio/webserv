#include "Cluster.hpp"

namespace ws
{
    Cluster::Cluster(Configuration const& config) : _config(config), _running(false), _setup(false)
    {
    }

    Cluster::~Cluster()
    {
    }

    void Cluster::setup()
    {
        std::vector<VServer*> const& servers = _config.getVServers();

        std::vector<VServer*>::const_iterator it = servers.begin();
        for (; it != servers.end(); it++)
        {
            (*it)->start(_binded_listens);
            // _fd_to_vserver[(*it)->getFd()] = *it;
            _server_fds.insert((*it)->getFd());
        }
        _setup = true;
    }

    /// helper functions ///
    inline bool Cluster::isServerFd(int fd)
    {
        return _server_fds.find(fd) != _server_fds.end();
    }

    /// Event handlers ///
    void Cluster::connectionHandler(int fd_index)
    {
        _client_fd = accept(_pollfds[fd_index].fd, (struct sockaddr*)&_client_addr, &_client_addr_len);
        fcntl(_client_fd, F_SETFL, O_NONBLOCK);
        if (_client_fd == -1)
            throw std::runtime_error("Cluster::run() : accept() failed");
        else
        {
            _pollfds[_nfds].fd = _client_fd;
            _pollfds[_nfds].events = POLLIN;
            _pollfds[_nfds].revents = 0;
            _nfds++;
        }
    }

    void Cluster::requestHandler(int fd_index)
    {
        if (_fd_to_request.find(_client_fd) == _fd_to_request.end())
        {
            Request request(_client_fd);
            _fd_to_request[_client_fd] = &request;
        }

        _fd_to_request[_client_fd]->process();

        if (_fd_to_request[_client_fd]->isComplete())
        {
            if (_fd_to_response.find(_client_fd) == _fd_to_response.end())
            {
                Response response(*_fd_to_request[_client_fd]);
                _fd_to_response[_client_fd] = &response;
            }
            _fd_to_response[_client_fd]->setup();
        }
        _pollfds[fd_index].events = POLLOUT;
        _pollfds[fd_index].revents = 0;
    }

    void Cluster::responseHandler(int fd_index)
    {
        //
        std::string responseBody = "Hello World\r\n";
        size_t      responseBodySize = responseBody.length();
        std::string responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + SSTR(responseBodySize) + "\r\n\r\n";
        std::string response = responseHeader + responseBody;
        if (send((_pollfds[fd_index].fd), response.c_str(), response.length(), 0) < 0)
        {
            perror("failed to send data");
            exit(1);
        }
        close(_pollfds[fd_index].fd);
        _pollfds[fd_index].fd = -1;
    }

    void Cluster::initPollFds()
    {
        std::vector<VServer*> const& servers = _config.getVServers();

        _nfds = 0;
        memset(&_pollfds, 0, sizeof(_pollfds));
        for (size_t i = 0; i < servers.size(); i++)
        {
            std::vector<struct Listen>::const_iterator it = servers[i]->getListens().begin();
            for (; it != servers[i]->getListens().end(); it++)
            {
                if (it->fd == -1)
                    continue;
                _pollfds[_nfds].fd = it->fd;
                _pollfds[_nfds].events = POLLIN;
                _pollfds[_nfds].revents = 0;
                _nfds++;
                _server_fds.insert(it->fd);
                std::cout << "fd: " << it->fd << " " << it->host << ":" << it->port << std::endl;
            }
        }
    }

    /// Main loop ///
    void Cluster::run()
    {
        if (_setup == false)
            throw std::runtime_error("Cluster::run() : setup() must be called before run()");

        initPollFds();

        console.log("Cluster started");
        _running = true;
        while (_running)
        {
            std::cout << "polling..." << std::endl;
            int ret = poll(_pollfds, _nfds, -1);
            if (ret == -1)
                throw std::runtime_error("Cluster::run() : poll() failed");
            else if (ret == 0)
                continue;

            for (size_t i = 0; i < _nfds; i++)
            {
                if (_pollfds[i].revents & POLLIN)
                {
                    if (isServerFd(_pollfds[i].fd))
                        connectionHandler(i);
                    else
                        requestHandler(i);
                }
                else if (_pollfds[i].revents & POLLOUT)
                    responseHandler(i);
            }
        } // while
    }

} // namespace ws