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

    void Cluster::run()
    {
        if (_setup == false)
            throw std::runtime_error("Cluster::run() : setup() must be called before run()");

        initPollFds();

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd;

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
                    if (_server_fds.find(_pollfds[i].fd) != _server_fds.end()) // fd is for server?
                    {
                        client_fd = accept(_pollfds[i].fd, (struct sockaddr*)&client_addr, &client_addr_len);
                        fcntl(client_fd, F_SETFL, O_NONBLOCK);
                        if (client_fd == -1)
                            throw std::runtime_error("Cluster::run() : accept() failed");
                        else
                        {
                            _pollfds[_nfds].fd = client_fd;
                            _pollfds[_nfds].events = POLLIN;
                            _pollfds[_nfds].revents = 0;
                            _nfds++;
                        }
                    }
                    else //  handle request
                    {
                        std::cout << "Reading..." << std::endl;
                        char buf[1024];
                        int ret = read(_pollfds[i].fd, buf, sizeof(buf));
                        if (ret == -1)
                        {
                            if (errno == EAGAIN || errno == EWOULDBLOCK)
                                continue;
                            else
                                throw std::runtime_error("Cluster::run() : read() failed");
                        }
                        else if (ret == 0)
                        {
                            std::cout << "Client disconnected" << std::endl;
                            close(_pollfds[i].fd);
                        }
                        else
                        {
                            
                            std::cout << "REQ: " << ret << "\n-------------------------" << std::endl;
                            std::cout << buf << std::endl;
                            std::cout << "-------------------------" <<std::endl;
                            _pollfds[i].events = POLLOUT;
                            _pollfds[i].revents = 0;
                        }
                    }

                }
                else if (_pollfds[i].revents & POLLOUT)
                {
                    std::string responseBody = "Hello World\r\n";
                    size_t      responseBodySize = responseBody.length();
                    std::string responseHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + SSTR(responseBodySize) + "\r\n\r\n";
                    std::string response = responseHeader + responseBody;
                    if (send((_pollfds[i].fd), response.c_str(), response.length(), 0) < 0)
                    {
                        perror("failed to send data");
                        exit(1);
                    }
                    close(_pollfds[i].fd);
                    _pollfds[i].fd = -1;
                }
            } //for
        } // while
    } // void Cluster::run()

} // namespace ws