#pragma once

#include "../Configuration/Parser.hpp"
#include "Socket.hpp"

typedef std::vector<std::string> t_vec_str;

namespace ws
{
    class VServer : protected Socket
    {
    public:
        VServer(parser::Context const &context);
        ~VServer();

        std::vector<struct Listen> const & getListens() const;

        std::map<std::string, struct Location> const& getLocations() const;
        bool hasName() const;
        std::string const &getName() const;
        int getIndex() const;
        std::set<int> const& getFds() const;

        t_vec_str const &get(const std::string &key) const;
        std::string const& getErrorPage(int code) const;

        void start(std::vector<struct Listen> & _binded_listens);
        void print() const;

        void prepareServerConfig(parser::Context const &context);

        struct Location const &resolveLocation(std::string path) const;

    private:
        int _ctx_index; // index of the server in the configuration file
        std::vector<struct Listen> _listens;
        std::map<std::string, port_t> _hostPortMap;
        std::map<const std::string, t_vec_str> _config;
        std::map<const int, std::string> _customErrorsPages;
        std::map<std::string, struct Location> _locations;
        struct sockaddr_in addr;
        bool            _started;

        std::set<int> _server_fds;

        void            _checkConfig(parser::Context const& context) const;
        void            setupListen(t_vec_str const& args);
    };
} // namespace ws