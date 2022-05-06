#pragma once

#include <map>

#include "Context.hpp"
#include "Parser.hpp"
#include "../Networking/VServer.hpp"
#include "webserv.hpp"

#include <stdlib.h>

typedef unsigned short port_t;

namespace ws
{
    // struct Listen;
    class VServer;
    class Configuration
    {
    public:
        Configuration(const std::string &path);
        ~Configuration();

        void setup();
        void print() const;
        std::vector<ws::VServer *> const & getVServers() const;
        std::map<port_t, struct ServerName> const & getServerNamesMap() const;

    private:
        std::vector<parser::Context> _contexts;
        std::vector<VServer *> _vservers;
        std::map<port_t, struct ServerName> _serverNamesMap;
        std::string _path;

        void parse();
        void prepare();
    };
}
