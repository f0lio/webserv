
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
            if (n == 0)
                break;
            if (n == -1)
            {
                if (errno == EAGAIN)
                {
                    std::cout << "Request::parseHeader() : EAGAIN" << std::endl;
                    break;
                }
                else
                    throw std::runtime_error("Request::parseHeader() : read() failed");
                throw std::runtime_error("Request::parseHeader() : read() failed");
            }
            buffer[n] = '\0';
            std::cout << "[" << buffer << "]" << std::endl;
            _request.append(buffer, n);
        }
        std::cout << "Request::parseHeader() : " << _request << std::endl;
        std::string::size_type pos = _request.find("\r\n\r\n");
        if (pos == std::string::npos)
            return;
        _header = _request.substr(0, pos);
        _body = _request.substr(pos + 4);
        _isHeaderSet = true;
    }
    

    void Request::process()
    {
        if (this->_isHeaderSet == false)
            this->parseHeader();
        // else
        // {
        //     if (this->_isChunked)
        //     {
        //         int n = ::read(fd, buffer, REQUEST_BUFFER_SIZE);
        //         if (n > 0)
        //         {
        //             this->_body.append(buffer, n);
        //             char *delim = "0\r\n\r\n";
        //             if (this->_body.find(delim) != std::string::npos)
        //             {
        //                 this->_isDone = true;
        //             }

        //         }
        //         else
        //         {
        //             this->_isDone = true;
        //         }
        //     }
        //     else
        //     {
        //         this->_isDone = true;
        //     }

        // }   
        // _isDone = true;
    }


    
} // namespace ws

