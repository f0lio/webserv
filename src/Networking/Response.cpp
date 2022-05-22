
#include "Response.hpp"

namespace ws
{

    Response::Response(Request const &request) : _request(request)
    {
    }

    Response::~Response()
    {
    }

    std::string const &Response::getBody() const
    {
        return _body;
    }

    std::string const &Response::getHeader() const
    {
        return _header;
    }

    std::string const &Response::getStatus() const
    {
        return _status;
    }

    void Response::process()
    {
        std::cout << "Response::process() : " << _response << std::endl;
        write(_request.getFd(), _response.c_str(), _response.size());
    }

    void Response::setup()
    {
        _status = "HTTP/1.1 200 OK\r\n";
        _header = "Content-Type: text/html\r\n";
        _body = "<html><body><h1>Hello World</h1></body></html>";
    }    
} // namespace ws
