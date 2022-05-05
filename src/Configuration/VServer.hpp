#pragma once

#include "Parser.hpp"
#include "webserv.hpp"

typedef std::vector<std::string> t_vec_str;

namespace ws
{
    // struct Listen;
    class VServer
    {
    public:
        VServer(parser::Context const &context);
        ~VServer();

        t_vec_str const &get(const std::string &key) const;
        std::vector<struct Listen> const &getListens() const;
        std::map<std::string, struct Location> const& getLocations() const;
        void print() const;

    private:
        std::vector<struct Listen> _listens;
        std::map<const std::string, t_vec_str> _config;
        std::map<std::string, struct Location> _locations;
    };
} // namespace ws