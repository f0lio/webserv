
#include "Response.hpp"

namespace ws
{
    Response::Response(Request const& request, const Configuration& config)
        : _request(request), _config(config), _isProcessed(false), _isSent(false)
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

        console.log("Formating response...");

        //print request
        console.log("Request: ");
        std::cout << "-------------------------------------------------" << std::endl;
        std::cout << "[" << _request.getPath() << "]" << std::endl;
        std::cout << "[" << _request.getHeader() << "]" << std::endl;
        std::cout << "-------------------------------------------------" << std::endl;

        int status = this->precheck(this->_request);

        if (status == 301)
        {
            const VServer& vs = *_request.resolveVServer();
            const struct Location& loc = vs.resolveLocation(_request.getPath());

            this->_status = "HTTP/1.1 301 Moved Permanently";
            this->_body = g_errorPages.at(status);
            this->_header = "Location: " + loc.config.at("redirect")[0] + CRLF;
            this->_header += "Content-Type: text/html" CRLF;
            this->_header += "Content-Length: " + SSTR(this->_body.size()) + CRLF;
            // this->_header += "Connection: close\r\n";
        }
        else if (status == -1) // autoindex is on
        {
            const VServer& vs = *_request.resolveVServer();
            const struct Location& loc = vs.resolveLocation(_request.getPath());

            console.log("Autoindexing...");

            _status = "200 OK";
            _header = "Content-Type: text/html\r\n";
            _body = ::autoIndex(loc.config.at("root")[0], _request.getPath());

        }
        else if (status)
        {
            console.err("Precheck failed");
            _status = "HTTP/1.1 " + SSTR(status) + " " + g_statusMessages.at(status);
            _header = "Content-Type: text/html\r\n";
            _body = g_errorPages.at(status);
        }
        else
        {

            const VServer& vs = *_request.resolveVServer();
            const struct Location& loc = vs.resolveLocation(_request.getPath());

            std::string path;
            try
            {
                path = loc.config.at("root")[0] + _request.getPath();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (file.is_open())
            {
                std::stringstream ss;
                ss << file.rdbuf();
                _body = ss.str();
                file.close();
                _status = "HTTP/1.1 200 OK";
                _header = "Content-Type: text/html\r\n";
            }
            else
            {
                console.err("[" + path + "] not found");
                _status = "HTTP/1.1 404 Not Found";
                _header = "Content-Type: text/html\r\n";
                _body = g_errorPages.at(404);
            }
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

    int Response::precheck(Request const& req) // TODO: WIP: implement functionlity in request
    {
        /*
        ** TODO:
        **  - get vs, and loc from request
        */

        // if (req.getPath().back() != '/')
        //     return 301;

        console.warn("Finding VServer...");
        const VServer& vs = *_request.resolveVServer();
        
        console.warn("Finding location...");
        const struct Location& loc = vs.resolveLocation(_request.getPath());

        for (auto const& it : loc.config)
        {
            console.log("=> ", it.first, ": ");
            for (auto const& it2 : it.second)
            {
                console.log("\t", it2);
            }
            std::cout << std::endl;
        }

        console.warn("Checking if should redirect...");
        if (req.getPath() != "/" && req.getPath().back() != '/' && loc.path != req.getPath())
        {
            std::cout << "------------------------------" << std::endl;
            std::cout << "Redirecting ..." << std::endl;
            std::cout << "Path: " << req.getPath() << std::endl;
            std::cout << "Location: " << loc.path << std::endl;
            std::cout << "------------------------------" << std::endl;

            return 301; // redirect to the correct location
        }

        try
        {
            std::cout << std::endl;
            std::cout << "=> LOCATION ROOT: [" << loc.config.at("root")[0] << "]" << std::endl;
            std::cout << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        const std::string& root = loc.config.at("root")[0];
        const std::string path = root + req.getPath();

        std::cout << "=> ROOT: [" << root << "]" << std::endl;
        std::cout << "=> PATH: [" << path << "]" << std::endl;

        if (loc.config.find("index") != loc.config.end())
        {
            t_vec_str::const_iterator it;
            for (it = loc.config.at("index").begin(); it != loc.config.at("index").end(); ++it)
            {
                std::string indexPath = path + *it;
                if (access(indexPath.c_str(), F_OK) == -1)
                    continue;
                if (access(indexPath.c_str(), R_OK) == -1)
                {
                    console.warn("[" + indexPath + "] is not readable.");
                    return 403; // forbidden
                }
                return 0; // index found
            }
        }

        if (loc.config.find("autoindex") == loc.config.end())
        {
            if (loc.config.at("autoindex")[0] == "on")
                return -1; // autoindex is on
        }
        return 404; // not found
    }
}