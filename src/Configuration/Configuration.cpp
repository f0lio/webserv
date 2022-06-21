#include "Configuration.hpp"

namespace ws
{
    Configuration::Configuration(const std::string& path) : _path(path)
    {
    }

    Configuration::~Configuration()
    {
        for (size_t i = 0; i < _vservers.size(); i++)
            delete _vservers[i];
    }

    void Configuration::parse()
    {
        std::ifstream file;
        file.open(_path.c_str());
        if (!file.is_open())
            throw std::runtime_error("Can't open file: " + _path);

        parser::Tokenizer tokenizer(file);
        parser::Parser parser(tokenizer);

        parser.parse();
        _contexts = parser.getContexts();
        file.close();
    }

    void Configuration::prepare()
    {
        for (size_t i = 0; i < _contexts.size(); i++)
        {
            parser::Context& ctx = _contexts[i];
            ctx.prepare();
        }
    }

    void Configuration::setup()
    {
        this->parse();
        this->prepare();

        for (size_t i = 0; i < _contexts.size(); i++)
            _vservers.push_back(new VServer(_contexts[i]));

        struct sockaddr_in addr;

        for (size_t i = 0; i < _vservers.size(); i++)
        {
            std::vector<std::string> const& serverNames = _vservers[i]->get("server_name");
            if (serverNames.size() == 0)
                throw std::runtime_error("server block " + SSTR(_vservers[i]->getIndex())
                    + " doesn't have any server_name directive");

            std::vector<struct Listen>::const_iterator it = _vservers[i]->getListens().begin();
            for (; it != _vservers[i]->getListens().end(); it++)
            {
                // TODO: handle ANY_ADDRESS

                // no need to check as it is already checked in VServer
                inet_aton(it->host.c_str(), &addr.sin_addr);

                for (size_t j = 0; j < serverNames.size(); j++)
                {
                    if (_serversTree[addr.sin_addr.s_addr][it->port].vservers[serverNames[j]] != NULL)
                        throw std::runtime_error( "server block " + SSTR(i)
                            + ": server_name \"" + serverNames[j] + "\" already exists");
                    else if (_serversTree[addr.sin_addr.s_addr][it->port].vservers[DEFAULT_SERVER_KEY] == NULL)
                        _serversTree[addr.sin_addr.s_addr][it->port].vservers[DEFAULT_SERVER_KEY] = _vservers[i];
                    _serversTree[addr.sin_addr.s_addr][it->port].vservers[serverNames[j]] = _vservers[i];
                }
                
                // DEBUG: tmp_map is used to find hostname by ip
                _tmp_map[addr.sin_addr.s_addr] = it->host;
            }
        }
    }

    void Configuration::print() const
    {
        console.log("Resolver tree:", "\n");

        std::map<in_addr_t, std::map<port_t, struct ServerName> >::const_iterator it;
        for (it = _serversTree.begin(); it != _serversTree.end(); it++)
        {
            sockaddr_in addr_;
            inet_aton(_tmp_map.find(it->first)->second.c_str(), &addr_.sin_addr);
            
            console.log("	IP: ", _tmp_map.find(it->first)->second, ": ", addr_.sin_addr.s_addr, "\n");
            


            console.log("		Ports:", "\n");
            std::map<port_t, struct ServerName>::const_iterator it2;
            for (it2 = it->second.begin(); it2 != it->second.end(); it2++)
            {
                console.log("			Port: ", it2->first, "\n");
                console.log("			ServerNames:", "\n");
                std::map<std::string, VServer*>::const_iterator it3;
                for (it3 = it2->second.vservers.begin(); it3 != it2->second.vservers.end(); it3++)
                {
                    console.log("				ServerName: ", it3->first);
                    console.log(": ", it3->second->getIndex());
                    if (it3->second->hasName())
                        console.log(" (", it3->second->getName(), ")");
                    console.log("\n");
                }
            }
            console.log("\n");
        }
    }

    std::vector<VServer*> const& Configuration::getVServers() const
    {
        return _vservers;
    }

    std::map<port_t, struct ServerName> const& Configuration::getServerNamesMap() const
    {
        return this->_serverNamesMap;
    }

    // const for speed (is it?)
    VServer *Configuration::getVServer(const in_addr_t addr, const port_t port, const std::string& server_name) const
    {
        std::map<in_addr_t, std::map<port_t, struct ServerName> >::const_iterator it = _serversTree.find(addr);
        if (it == _serversTree.end())
            return NULL;

        std::map<port_t, struct ServerName>::const_iterator it2 = it->second.find(port);
        if (it2 == it->second.end())
            return NULL;

        std::map<std::string, VServer*>::const_iterator it3 = it2->second.vservers.find(server_name);
        if (it3 == it2->second.vservers.end())
            return NULL;

        return it3->second;
    }

    VServer *Configuration::getVServer(const std::string &ip, const port_t port, const std::string& server_name) const
    {
        in_addr_t addr = inet_addr(ip.c_str());
        return this->getVServer(addr, port, server_name);
    }
} // namespace ws
