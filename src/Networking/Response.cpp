
#include "Response.hpp"

namespace ws
{
    Response::Response(Request const &request, const Configuration &config)
        : _request(request), _config(config), _isProcessed(false), _isSent(false)
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
        if (isProcessed())
            return;

        console.log("Formating response...");

        if (_request.getStatus() != 200)
        {
            _status = "HTTP/1.1 " + SSTR(_request.getStatus()) + " " + g_statusMessages.at(_request.getStatus());
            _header = "Content-Type: text/html\r\n";
            _body = g_errorPages.at(_request.getStatus());
        }
        else
        {
            // TODO: get vs from request, as it already has been resolved
            const VServer &vs = *_request.resolveVServer();
            std::cout << vs.getName() << std::endl;

            // vs.
            
            this->_status = "HTTP/1.1 " + SSTR(_request.getStatus()) + " " + g_statusMessages.at(_request.getStatus());
            this->_header = "Content-Type: text/html; charset=UTF-8";
            // styled html with css
            this->_body =
            _request.getBody();
                // "<!DOCTYPE html><html><head><title>" + vs.getName() + "</title><style>body{background-color: #ddd;font-size: 1em;color: #333;margin: 0;padding: 5px 5px ;}</style>" + vs.getName() + " : " + SSTR(vs.getIndex()) + "</h1></body></html>";
        }

        _response = _status + "\r\n" + _header + "\r\n\r\n" + _body;
        _isProcessed = true;
        console.log("Response formated.");
    }

    void Response::send()
    {
        ::send(_request.getClientFd(), _response.c_str(), _response.size(), 0);
        console.log("Response sent.");
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

    /* [WIP - waiting for request to be refactored]
    **
    ** This function is called just before reading the body of the request
    ** It checks if the request is valid based on config file.
    ** RETURN:
    **  zero if the body-independent checks are ok,
    **  otherwise, it returns the coresponding status code
    **/

    int Response::precheck(Request const &req) // TODO: WIP: implement functionlity in request
    {
        // const VServer* vs = request.getVServer();

        // is:
        //  - the request method is valid/allowed
        //  - auth
        //  - resource exists
        // ["OPTIONS","GET","HEAD","PUT","POST","DELETE","PATCH"]
        // int i;
        // for (i = 9; i > -1; i--)
        //     if (req.getMethod() != g_validMethods[i])
        //         break;
        // if (i != -1)
        //     return 405; //

        //...
        return 0;
    }
}
