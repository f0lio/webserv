#pragma once

#include <map>
#include "./Context.hpp"
#include "./Parser.hpp"
#include "./VServer.hpp"

namespace ws
{
    class VServer;
    class Configuration
    {
    public:
        Configuration(const std::string &path);
        ~Configuration();

        void parse();
        void setup();
        void print() const;
        std::vector<ws::VServer *> const & getVServers() const;

    private:
        std::vector<parser::Context> _contexts;
        std::vector<VServer *> _vservers;
        std::string _path;
    };
}
