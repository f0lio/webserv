#pragma once

#include "../Configuration/Parser.hpp"
#include "../Configuration/Context.hpp"
#include "Socket.hpp"
#include "webserv.hpp"

typedef std::vector<std::string> t_vec_str;

namespace ws
{
    class VServer : protected Socket
    {
    public:
        VServer(parser::Context const &context);
        ~VServer();

        t_vec_str const &get(const std::string &key) const;
        std::vector<struct Listen> const &getListens() const;
        std::map<std::string, struct Location> const& getLocations() const;
        int getFd() const;
        void start();
        void print() const;

    private:
        std::vector<struct Listen> _listens;
        std::map<const std::string, t_vec_str> _config;
        std::map<std::string, struct Location> _locations;

        struct sockaddr_in addr;
    };
} // namespace ws