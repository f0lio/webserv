#include "VServer.hpp"

namespace ws
{
    bool isAlreadyBinded(struct Listen const& listen, std::vector<struct Listen>& _binded_listens, int* _fd);

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
            {
                // if (parser::directiveRulesMap[dirs[i].getKey()].occurrence == parser::OCCURENCE_MULTIPLE)
                //     _config[dirs[i].getKey()] = dirs[i].getArgs();
                // else
                _config[dirs[i].getKey()] = dirs[i].getArgs();
            }
        }

        if (_config.find("redirect") == _config.end())
            _config["redirect"].push_back("/");

        for (size_t i = 0; i < locs.size(); i++)
        {
            struct Location loc;
            std::vector<parser::SimpleDirective> const& dirs = locs[i].getDirectives();
            for (size_t j = 0; j < dirs.size(); j++)
                loc.config[dirs[j].getKey()] = dirs[j].getArgs();

            loc.path = locs[i].getArgs()[0];
            if (loc.config.find("root") == loc.config.end())
                loc.config["root"] = _config["root"];
            if (loc.config.find("redirect") == loc.config.end())
                loc.config["redirect"] = _config["redirect"];
            _locations[loc.path] = loc;

        }
        if (_listens.size() == 0)
            throw std::runtime_error(
                "server block " + SSTR(context.getIndex()) + " doesn't have any listen directive");
        // _checkConfig(context);
        prepareServerConfig(context);
    }

    VServer::~VServer()
    {
        for (size_t i = 0; i < _listens.size(); i++)
            ::close(_listens[i].fd);
    }

    void VServer::prepareServerConfig(parser::Context const& context)
    {

        bool foundRoot = false;
        for (
            std::map<std::string, struct Location>::iterator it = _locations.begin();
            it != _locations.end();
            it++
            )
        {
            struct Location& loc = it->second;
            if (loc.path == "/")
            {
                foundRoot = true;
                break;
            }
        }


        if (!foundRoot)
        {
            struct Location loc;
            loc.path = "/";
            std::map<const std::string, t_vec_str>::iterator it;
            for (it = _config.begin(); it != _config.end(); it++)
            {
                if (locationDirectiveRulesMap.find(it->first) != locationDirectiveRulesMap.end())
                    loc.config[it->first] = it->second;
            }
            _locations["/"] = loc;
        }

        // for (std::map<int, std::string>::iterator it = g_errorPages.begin(); it != g_errorPages.end(); it++)
        // {
        //     // if (this->get("error_page")[it->first] == "")
        //     //     continue;
        // }
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

    const std::vector<struct Listen>& VServer::getListens() const
    {
        return _listens;
    }

    std::set<int> const& VServer::getFds() const
    {
        return _server_fds;
    }

    std::string const& VServer::getName() const
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
        listen.fd = -1; // -1 means not binded yet
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
                "server block " + SSTR(_ctx_index) + ": invalid port number: " + SSTR(listen.port));

        if (_hostPortMap[listen.host] == listen.port)
            throw std::runtime_error(
                "server block " + SSTR(_ctx_index) + ": duplicate listen " + listen.host + ":" + SSTR(listen.port));
        _hostPortMap[listen.host] = listen.port;
        if (inet_aton(listen.host.c_str(), &addr.sin_addr) == 0) // checking if host is valid
            throw std::runtime_error(
                "server block " + SSTR(_ctx_index) + ": invalid host: " + listen.host);
        listen.addr_in.sin_addr.s_addr = addr.sin_addr.s_addr;
        _listens.push_back(listen);
    }

    // void VServer::start(std::map<in_addr_t, std::vector<port_t> >& _binded_listens)
    void VServer::start(std::vector<struct Listen>& _binded_listens)
    {
        for (size_t i = 0; i < _listens.size(); i++)
        {
            int binded_fd;
            if (isAlreadyBinded(_listens[i], _binded_listens, &binded_fd))
            {
                _server_fds.insert(binded_fd);
                continue;
            }
            _listens[i].addr_in.sin_family = AF_INET;
            if (_listens[i].host == "0.0.0.0")
                _listens[i].addr_in.sin_addr.s_addr = INADDR_ANY;
            else
                _listens[i].addr_in.sin_addr.s_addr = inet_addr(_listens[i].host.c_str());

            _listens[i].addr_in.sin_port = htons(_listens[i].port);

            int sock_len = sizeof(_listens[i].addr_in);

            std::string formated_listen = _listens[i].host + ":" + SSTR(_listens[i].port);

            // signal(SIGPIPE, SIG_IGN);
            if ((this->_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
                throw std::runtime_error("Could not create socket for " + formated_listen);

            // int optval = 1;
            // setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

            _listens[i].fd = this->_fd;
            if (::bind(
                this->_fd,
                (struct sockaddr*)&_listens[i].addr_in,
                (socklen_t)sock_len) == -1)
                throw std::runtime_error(
                    "Could not bind socket to address " + formated_listen);

            if (::listen(this->_fd, BACK_LOG) == -1)
                throw std::runtime_error("Could not listen on socket for " + formated_listen);

            _server_fds.insert(_listens[i].fd);
            _binded_listens.push_back(_listens[i]);
        }
        _started = true;
    }

    struct Location const& VServer::resolveLocation(std::string path) const
    {
        std::map<std::string, struct Location>::const_iterator it;

        while (path.size() > 0)
        {
            console.warn("resolveLocation: [" + path + "]");
            it = _locations.find(path);
            if (it != _locations.end())
                return it->second;
            if (path.size() > 1 && path[path.size() - 1] == '/')
                path.erase(path.size() - 1);
            size_t pos = path.find_last_of('/');
            if (pos == std::string::npos)
                break;
            path = path.substr(0, pos + 1);
        }
        it = _locations.find(path);

        if (it != _locations.end())
            return it->second;
        else
            return _locations.find("/")->second;
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
    bool isAlreadyBinded(
        struct Listen const& listen,
        std::vector<struct Listen>& _binded_listens,
        int* _fd)
    {
        for (size_t i = 0; i < _binded_listens.size(); i++)
        {
            if (_binded_listens[i].addr_in.sin_addr.s_addr == listen.addr_in.sin_addr.s_addr && _binded_listens[i].port == listen.port)
            {
                *_fd = _binded_listens[i].fd;
                return true;
            }
        }
        return false;
    }
} // namespace ws
