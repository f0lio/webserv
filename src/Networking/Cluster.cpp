#include "Cluster.hpp"

namespace ws
{
    Cluster::Cluster(Configuration const& config) : _config(config)
    {
    }

    Cluster::~Cluster()
    {
    }

    // void Cluster::setup()
    // {
    //     int yes = 1;
    //     int server_fd = 0;
    //     std::vector<Listen> is_bind;

    //     for (std::vector<ServerConfig>::iterator it = servers_.begin(); it != servers_.end(); it++)
    //     {

    //         if (it->getListens().empty())
    //             it->getListens().push_back(Listen("0.0.0.0", 80));

    //         for (std::vector<Listen>::iterator list = it->getListens().begin(); list != it->getListens().end(); list++)
    //         {

    //             if (std::find(is_bind.begin(), is_bind.end(), *list) == is_bind.end())
    //             {
    //                 if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    //                     throw webserv_exception("socket() failed", errno);

    //                 fcntl(server_fd, F_SETFL, O_NONBLOCK);

    //                 struct sockaddr_in address;
    //                 memset(&address, 0, sizeof(address));
    //                 address.sin_family = AF_INET;
    //                 address.sin_addr.s_addr = inet_addr(list->ip_.c_str());
    //                 address.sin_port = htons(list->port_);

    //                 setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    //                 if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    //                     throw webserv_exception("bind() to % failed", errno, list->ip_ + ":" + ft::to_string(list->port_));

    //                 if (listen(server_fd, MAX_CONNECTION) < 0)
    //                     throw webserv_exception("listen() failed", errno);

    //                 running_server_[server_fd] = Listen(list->ip_, list->port_);

    //                 Log.print(INFO, "listening on " + ft::to_string(list->ip_) + ":" + ft::to_string(list->port_), GREEN);

    //                 add_to_fd_set(server_fd);
    //                 is_bind.push_back(*list);
    //             }
    //         }
    //     }

    //     if (is_bind.empty())
    //         throw webserv_exception("listen() failed", errno);
    // }

    void Cluster::setup()
    {
        std::vector<VServer *> const & servers = _config.getVServers();
        for (std::vector<VServer *>::const_iterator it = servers.begin(); it != servers.end(); it++)
        {
            (*it)->start();
        }


    }

    void Cluster::run()
    {
        std::map<port_t, struct ServerName> const& serverNamesMap = _config.getServerNamesMap();
        std::vector<VServer *> const & servers = _config.getVServers();
        
        _nfds = 0;
        
        for (size_t i = 0; i < servers.size(); i++)
        {
            (_pollfds + i)->fd = servers[i]->getFd();
            (_pollfds + i)->events = POLLIN;
            (_pollfds + i)->revents = 0;
            _nfds++;
        }
        console.log("Cluster started");
        // while (1)
        // {
        //     // call poll.
        //     if (poll(_pollfds, _nfds, -1) < 0)
        //         throw std::runtime_error("poll() failed");
        //     for (size_t i = 0; i < _nfds; i++)
        //     {
        //         if (_pollfds[i].revents & POLLIN)
        //         {
        //             if (_pollfds[i].fd == _listen_fd)
        //             {
        //                 // new connection.
        //             }

        //         }
        //         else if (_pollfds[i].revents & POLLOUT)
        //         {

        //         }
        //     }   
        // }
    }
}