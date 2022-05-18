#include "VServer.hpp"

namespace ws
{
    bool isAlreadyBinded(struct Listen const& listen, std::map<in_addr_t, std::vector<port_t> >& _binded_listens);

    VServer::VServer(parser::Context const& context) : _started(false)
    {
        std::vector<parser::SimpleDirective> const& dirs = context.getSimpleDirectives();
        std::vector<parser::BlockDirective> const& locs = context.getBlockDirectives();
        _ctx_index = context.getIndex();

        struct sockaddr_in addr;
        for (size_t i = 0; i < dirs.size(); i++)
        {
            if (dirs[i].getKey() == "listen")
                setupListen(dirs[i].getArgs());
            else
                _config[dirs[i].getKey()] = dirs[i].getArgs();
        }
        for (size_t i = 0; i < locs.size(); i++)
        {
            struct Location loc;
            std::vector<parser::SimpleDirective> const& dirs = locs[i].getDirectives();
            for (size_t j = 0; j < dirs.size(); j++)
                loc.config[dirs[j].getKey()] = dirs[j].getArgs();

            loc.path = locs[i].getArgs()[0];

            if (loc.config.find("root") == loc.config.end())
                loc.config["root"] = _config["root"];
            _locations[loc.path] = loc;
        }
        if (_listens.size() == 0)
            throw std::runtime_error(
                "server block " + SSTR(context.getIndex())
                + " doesn't have any listen directive");
        // _checkConfig(context);
    }

    VServer::~VServer()
    {
    }

    t_vec_str const& VServer::get(const std::string& key) const
    {
        if (_config.find(key) != _config.end())
            return _config.find(key)->second;
        else
            return t_vec_str();
    }

    std::map<std::string, struct Location> const& VServer::getLocations() const
    {
        return _locations;
    }
    
    std::vector<int> const& VServer::getListenFds() const
    {
        return _listen_fds;
    }

    const  std::vector<struct Listen>& VServer::getListens() const
    {
        return _listens;
    }

    int VServer::getFd() const
    {
        return _fd;
    }

    std::string VServer::getName() const
    {
        std::map<const std::string, t_vec_str>::const_iterator it;
        it = _config.find("name");
        if (it != _config.end())
            return it->second[0];
        return "";
    }

    bool VServer::hasName() const
    {
        return _config.find("name") != _config.end();
    }

    int VServer::getIndex() const
    {
        return _ctx_index;
    }

    void VServer::_checkConfig(parser::Context const& context) const
    {
        // std::vector<parser::SimpleDirective> const& dirs = context.getSimpleDirectives();
        // std::vector<parser::BlockDirective> const& locs = context.getBlockDirectives();
    }

    void VServer::setupListen(t_vec_str const& args)
    {
        struct Listen listen;
        if (args.size() == 1)
        {
            listen.host = "0.0.0.0";
            if (is_number(args[0].c_str()))
                listen.port = std::atoi(args[0].c_str());
            else
                throw std::runtime_error("Invalid port number: " + args[0]);
        }
        else if (args.size() == 2)
        {
            listen.host = args[0];
            if (listen.host == "localhost")
                listen.host = "127.0.0.1";
            if (is_number(args[1].c_str()))
                listen.port = atoi(args[1].c_str());
            else
                throw std::runtime_error("Invalid port number: " + args[1]);
        }
        if (listen.port < 0 || listen.port > 65535) // 1024?
            throw std::runtime_error(
                "server block " + SSTR(_ctx_index)
                + ": invalid port number: " + SSTR(listen.port)
            );
        if (_hostPortMap[listen.host] == listen.port)
            throw std::runtime_error(
                "server block " + SSTR(_ctx_index)
                + ": duplicate listen " + listen.host + ":" + SSTR(listen.port)
            );
        _hostPortMap[listen.host] = listen.port;
        if (inet_aton(listen.host.c_str(), &addr.sin_addr) == 0) // checking if host is valid
            throw std::runtime_error(
                "server block " + SSTR(_ctx_index)
                + ": invalid host: " + listen.host
            );
        listen.addr = addr.sin_addr.s_addr;
        _listens.push_back(listen);
    }

    void VServer::start(std::map<in_addr_t, std::vector<port_t> >& _binded_listens)
    {
        std::cout << "Starting server " << getName() << std::endl;
        for (size_t i = 0; i < _listens.size(); i++)
        {
            if (isAlreadyBinded(_listens[i], _binded_listens))
                continue;
            else
                _binded_listens[_listens[i].addr].push_back(_listens[i].port);

            this->addr.sin_family = AF_INET;
            if (_listens[i].host == "0.0.0.0")
            {
                std::cout << this->getIndex() << " is listening on all interfaces" << std::endl;
                this->addr.sin_addr.s_addr = INADDR_ANY;
            }
            else
                this->addr.sin_addr.s_addr = inet_addr(_listens[i].host.c_str());

            this->addr.sin_port = htons(_listens[i].port);

            int sock_len = sizeof(this->addr);

            if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            {
                throw std::runtime_error("Could not create socket");
            }
            _listen_fds.push_back(this->_fd);

            if (::bind(
                this->_fd,
                (struct sockaddr*)&this->addr,
                (socklen_t)sock_len) == -1)
                throw std::runtime_error("Could not bind socket");

            if (::listen(this->_fd, BACK_LOG) == -1)
            {
                throw std::runtime_error("Could not listen on socket");
            }
            _started = true;
        }

        // std::cout << "Server " << this->getIndex() << " started on" << std::endl;
        //print listens
        // for (size_t i = 0; i < _listens.size(); i++)
        // {
        //     std::cout << _listens[i].host << ":" << _listens[i].port << std::endl;
        // }

    }

    void VServer::print() const
    {
        std::cout << "## VServer ##" << std::endl;

        for (size_t i = 0; i < _listens.size(); i++)
        {
            std::cout << "Listen: " << _listens[i].host << ":" << _listens[i].port << std::endl;
        }

        for (auto const& it : _config)
        {
            std::cout << it.first << ": ";
            for (auto const& it2 : it.second)
            {
                std::cout << it2 << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "-> Locations" << std::endl;
        for (auto const& it : _locations)
        {
            std::cout << it.first << ": ";
            for (auto const& it2 : it.second.config)
            {
                std::cout << it2.first << ": ";
                for (auto const& it3 : it2.second)
                {
                    std::cout << it3 << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    } // print

    // helper function(s?)
    bool isAlreadyBinded(struct Listen const& listen, std::map<in_addr_t, std::vector<port_t> >& _binded_listens)
    {
        std::map<in_addr_t, std::vector<port_t> >::iterator it3 = _binded_listens.find(listen.addr);
        if (it3 != _binded_listens.end())
            return std::find(it3->second.begin(), it3->second.end(), listen.port) != it3->second.end();
        return false;
    }
} // namespace ws
