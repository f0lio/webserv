#pragma once

#include "VServer.hpp"

namespace ws
{
    class Request
    {
    public:
        Request(int client_fd, std::vector<VServer*> & vservers);
        ~Request();

        std::string const &getHeader() const;
		std::string const &getHeaderField(std::string const &key) const;
        std::string const &getBody() const;
        std::string const &getMethod() const;
        std::string const &getPath() const;
        std::string const &getQuery() const;
        int const &getStatus() const;
        int const& getClientFd() const;

        std::vector<VServer*> & getVServers() const;

        void process();
        bool isComplete() const;

    private:
        int _client_fd;
        std::string _request;
        std::string _method;
        std::string _path;
        std::string _query;
        std::string _header;
        std::string _body;
        bool _isChunked = 0;
        bool _isDone = 0;
        size_t _content_length = -1;
		int	_status = 0;
        std::map<std::string, std::string> _headers;
        std::vector<VServer*> & _vservers;

        //
        int parseHeader();
        int processHeader();

        int parseBody();
        int chunkedBody();
    };

} // namespace ws
