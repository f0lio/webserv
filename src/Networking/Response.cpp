
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

    void Response::process()
    {
        if (isProcessed())
            return;

        console.log("Processing response");
        const struct sockaddr_in& client_addr = _request.getClientAddress();
        // VServer *Configuration::getVServer(const in_addr_t addr, const port_t port, const std::string& server_name)
        
         std::cout << client_addr.sin_addr.s_addr << " "
             << ntohs(client_addr.sin_port)
             << " bx.com" << std::endl;

        const VServer * vserver = _config.getVServer(
            client_addr.sin_addr.s_addr,
            ntohs(client_addr.sin_port),
            "bx.com"
        );
        console.log("Resolver is done");


        if (vserver == NULL)
        {
            console.log("No vserver found");
            _status = "404 Not Found";
            _body = "<html><body><h1>404 Not Found</h1></body></html>";
            _header = "HTTP/1.1 404 Not Found\r\n";
            _header += "Content-Type: text/html\r\n";
            _header += "Content-Length: " + SSTR(_body.size()) + "\r\n";
            _header += "Connection: close\r\n";
            _header += "\r\n";
            _response = _header + _body;
        }
        else
        {
            console.log("Found vserver");
            _status = "200 OK";
            if (vserver->hasName())
                _body = vserver->getName();
            else
                _body = SSTR(vserver->getIndex());
            _header = "HTTP/1.1 200 OK\r\n";
            _header += "Content-Type: text/html\r\n";
            _header += "Content-Length: " + SSTR(_body.size()) + "\r\n";
            _header += "Connection: close\r\n";
            _header += "\r\n";
            _response = _header + _body;
        }
        _isProcessed = true;
    }

    void Response::send()
    {
        ::send(_request.getFd(), _response.c_str(), _response.size(), 0);
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

