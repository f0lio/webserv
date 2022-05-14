#pragma once

#include <stdlib.h>
#include <arpa/inet.h>

#include <map>

#include "webserv.hpp"
#include "Context.hpp"
#include "Parser.hpp"
#include "../Networking/VServer.hpp"

typedef unsigned short port_t;

namespace ws
{
    struct Listen // compiler doesn't like my design, so I'm making it worse
    {
        in_addr_t addr;
        std::string host;
        port_t port;
    };
    class VServer;
    class Configuration
    {
    public:
        Configuration(const std::string& path);
        ~Configuration();

        void setup();
        void print() const;
        std::vector<ws::VServer*> const& getVServers() const;
        std::map<port_t, struct ServerName> const& getServerNamesMap() const;

    private:
        std::vector<parser::Context> _contexts;
        std::vector<VServer*> _vservers;
        std::map<port_t, struct ServerName> _serverNamesMap;

        // in_addr_t to avoid conversion at resolve()..
        // std::map<in_addr_t, std::map<port_t, struct ServerName> > _serversTree;
        std::map<in_addr_t, std::map<port_t, struct ServerName> > _serversTree;
        std::string _path;

        std::map<in_addr_t, std::string> _tmp_map;

        void parse();
        void prepare();
    };
}
