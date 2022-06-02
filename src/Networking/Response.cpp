
#include "Response.hpp"

namespace ws
{
    Response::Response(Request const& request, const Configuration& config)
        : _request(request)
        , _config(config)
        , _isProcessed(false)
        , _isSent(false)
    {
    }

    Response::~Response()
    {
    }

    std::string const& Response::getBody() const
    {
        return _body;
    }

    std::string const& Response::getHeader() const
    {
        return _header;
    }

    std::string const& Response::getStatus() const
    {
        return _status;
    }

    const VServer* Response::resolveVServer() const 
    {
        std::vector<VServer*>::iterator it = _request.getVServers().begin();

        std::string host =  "ax.com";//this->_request.getHeaderField("Host");

        for (; it != _request.getVServers().end(); ++it)
        {
            if (std::find((*it)->get("server_name").begin(),
                (*it)->get("server_name").end(), host) != (*it)->get("server_name").end())
                return *it;
        }
        return *_request.getVServers().begin();
    }

// #define MAX_PATH_LENGTH 256
// _statusMessage
// _errorMessage

    void Response::process()
    {
        if (isProcessed())
            return;
  
        const VServer* vserver = resolveVServer();

        console.log("Resolver is done");

        std::cout << vserver->getName() << std::endl;

        // if (_request.status != -1)
        // {
        //     _status = "HTTP/1.1 " + std::to_string(_request.status) + " " + _statusMessage[_status];
        //     _body = _errorMessage[_request.status];

        // }

        this->_status = "HTTP/1.1 200 OK";
        this->_header = "Content-Type: text/html; charset=UTF-8";
        this->_body = "<h1>Hello World!</h1>";

        _response = _status + "\r\n" + _header + "\r\n\r\n" + _body;
        _isProcessed = true;
    }

    void Response::send()
    {
        ::send(_request.getClientFd(), _response.c_str(), _response.size(), 0);
        _isSent = true;
    }

    bool Response::isSent() const
    {
        return _isSent;
    }
    
    bool Response::isProcessed() const
    {
        return _isProcessed;
    }
}
