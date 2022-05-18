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
        std::vector<struct Listen> const & getListens() const;

        std::map<std::string, struct Location> const& getLocations() const;
        int getFd() const;
        bool hasName() const;
        std::string getName() const;
        int getIndex() const;

        void start(std::map<in_addr_t, std::vector<port_t> > &_binded_listens);
        void print() const;

        void handleConnection(int client_fd);

    private:
        int _ctx_index; // index of the server in the configuration file
        std::vector<struct Listen> _listens;
        std::map<std::string, port_t> _hostPortMap;
        std::map<const std::string, t_vec_str> _config;
        std::map<std::string, struct Location> _locations;
        struct sockaddr_in addr;
        bool            _started;

        void            _checkConfig(parser::Context const& context) const;
        void            setupListen(t_vec_str const& args);
    };
} // namespace ws