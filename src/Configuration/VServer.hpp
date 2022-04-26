#pragma once

#include "Parser.hpp"
#include "webserv.hpp"

typedef std::vector<std::string> t_vec_str;

namespace ws
{
    class VServer
    {
    public:
        VServer(parser::Context const &context);
        ~VServer();

        t_vec_str const &get(const std::string &key) const;
        void print() const;

    private:
        std::map<const std::string, t_vec_str> _config;
        std::map<std::string, struct Location> _locations;
    };
} // namespace ws