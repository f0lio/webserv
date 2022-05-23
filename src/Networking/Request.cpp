
#include "Request.hpp"

namespace ws
{

    Request::Request(int client_fd) : _fd(client_fd), _isDone(false), _isChunked(false)
    {
    }
    
    Request::~Request()
    {
    }

    
    std::string const &Request::getHeader() const
    {
        return _header;
    }

    std::string const &Request::getBody() const
    {
        return _body;
    }

    std::string const &Request::getMethod() const
    {
        return _method;
    }

    std::string const &Request::getPath() const
    {
        return _path;
    }

    std::string const &Request::getQuery() const
    {
        return _query;
    }

    int const &Request::getFd() const
    {
        return _fd;
    }

    bool Request::isComplete() const
    {
        return _isDone;
    }

    void Request::parseHeader()
    {
        char buffer[REQUEST_BUFFER_SIZE];

        while (true)
        {
            int n = read(_fd, buffer, REQUEST_BUFFER_SIZE);
            if (n == 0 || n == -1)
                break;
            buffer[n] = '\0';
            _request.append(buffer, n);
            if (_request.find("\r\n\r\n") != std::string::npos)
                break;
        }
        std::string::size_type pos = _request.find("\r\n\r\n");
        if (pos == std::string::npos)
            return;
        _header = _request.substr(0, pos);
        _body = _request.substr(pos + 4);
        _isHeaderSet = true;
        // this->
    }

    void Request::parseBody()
    {
        if (_headers.find("Content-Length") == _headers.end())
            return;
        // else if (checkHeaderDirective("Content-Length", _headers["Content-Length"])
        // {
        //     _body = _request.substr(pos + 4);
        // }

        char buffer[REQUEST_BUFFER_SIZE];
        size_t content_length = atoi(_headers["Content-Length"].c_str());
        while (_body.size() < content_length)
        {
            int n = read(_fd, buffer, REQUEST_BUFFER_SIZE);
            if (n == 0 || n == -1)
                break;
            buffer[n] = '\0';
            _body.append(buffer, n);
        }
    }

    void Request::process()
    {
        if (this->_isHeaderSet == false)
            this->parseHeader();
        else
            this->parseBody();
    }
} // namespace ws
