
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

    void Response::process()
    {
        if (isProcessed())
            return;

        console.log("Formating response...");

        int status = 0;

        status = this->precheck(this->_request);

        const VServer& vs = _request.getVServer();
        const struct Location& loc = _request.getLoc();

        if (status == 301) // redirection
        {
            setLocation(loc.config.at("redirect")[0]);
            setStatus(status);
        }
        else if (status == -1) // autoindex is on
        {
            console.log("Autoindexing...");
            setBody(::autoIndex(loc.config.at("root")[0], _request.getPath()));
            setResponse(200, resolveContentType(_request.getPath()));
            
        }
        else if (status == 200) // success
        {
            console.log("Precheck passed");
            //open file and set body
            std::string filePath = loc.config.at("root")[0] + _request.getPath();
            console.log("File path: " + filePath);
            std::ifstream file;
            file.open(filePath.c_str());
            if (file.is_open())
            {
                console.log("File opened");
                std::stringstream buffer;
                buffer << file.rdbuf();
                setBody(buffer.str());
                setResponse(status, resolveContentType(filePath));
                file.close();
            }
            else
                setErrorResponse(404), console.err("File not found");
        }
        // else if (status >= 400 && status < 500)
        else if (status) // error
            setErrorResponse(status), console.err("Precheck failed");
        else
        {
            console.err("------------ inside ELSE -----------");
            std::cout << "status: " << status << std::endl;

            std::string path;
            std::string fileName;
            try
            {
                path = loc.config.at("root")[0] + _request.getPath();
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }

            console.warn("Inside process");

            resolveIndexFile(loc, path, &fileName);

            path = fileName;
            console.log("Opening file: " + path);

            std::ifstream file;
            file.open(path.c_str(), std::ios::in | std::ios::binary);
            if (file.is_open())
            {
                console.log("File opened");
                std::stringstream ss;
                ss << file.rdbuf();
                setBody(ss.str());
                // setBody(file);
                file.close();
                setResponse(200, resolveContentType(path));
            }
            else
            {
                console.err("[" + path + "] not found");
                try
                {
                    console.err("[" + path + "] not found");
                    setStatus(404); // ...
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
                setErrorResponse(404);
            }
        }
        endResponse();
    }

    /* [WIP - waiting for request to be refactored]
    **
    ** This function is called just before reading the body of the request
    ** It checks if the request is valid based on config file.
    ** RETURN:
    **  zero if the body-independent checks are ok,
    **  otherwise, it returns the coresponding status code
    **/
    int Response::precheck(Request const& req) // TODO: WIP: implement functionality(?) in request
    {
        /*
        ** TODO:
        **  - get vs, and loc from request
        */

        const VServer& vs = _request.getVServer();

        const struct Location& loc = _request.getLoc();

        // console.warn("Checking if should redirect...");
        // if (req.getPath() != "/" && req.getPath().back() != '/' && loc.path == req.getPath())
        // {
        //     std::cout << "------------------------------" << std::endl;
        //     std::cout << "Redirecting ..." << std::endl;
        //     std::cout << "Path: " << req.getPath() << std::endl;
        //     std::cout << "Location: " << loc.path << std::endl;
        //     std::cout << "------------------------------" << std::endl;

        //     return 301; // redirect to the correct location
        // }

        std::cout << "_request.getStatus(): " << _request.getStatus() << std::endl;

        if (_request.getStatus() != OK_200)
            return _request.getStatus();

        try
        {
            console.log("=> LOCATION ROOT: [" + loc.config.at("root")[0] + "]");
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        const std::string& root = loc.config.at("root").at(0);
        const std::string path = root + req.getPath();

        console.log("=> ROOT: [" + root + "]");
        console.log("=> PATH: [" + path + "]");

        if (file_exists(path) == false)
        {
            console.warn("[" + path + "] not found");
            return 404;
        }
        else if (is_regular_file(path))
        {
            console.warn("[" + path + "] is a regular file");
            return 200;
        }
        else if (loc.config.find("index") != loc.config.end())
        {
            std::string fileName;
            console.warn("Inside precheck()");
            int status = resolveIndexFile(loc, path, &fileName);
            if (status != 404)
            {
                console.log("Found index file: " + fileName);
                return status;
            }
        }
        else
        {
            if (is_directory(path))
            {
                console.warn("[" + path + "] is a directory");
                if (loc.config.find("autoindex") != loc.config.end())
                {
                    console.warn("checking if autoindex is on");
                    if (loc.config.at("autoindex")[0] == "on")
                    {
                        console.warn("+++ autoindex is ON +++");
                        return -1; // autoindex is on
                    }
                    console.warn("--- autoindex is OFF ---");
                }
                return 403; // forbidden
            }
        }
        return 404; // not found
    }

    void Response::send()
    {
        if (_isSent)
            return;
        ::send(_request.getClientFd(), _response.c_str(), _response.size(), 0);
        console.log("Response sent.");
        _isProcessed = true;
        _isSent = true; // TODO: need to check if the response is fully sent
    }

    bool Response::isSent() const
    {
        return _isSent;
    }

    bool Response::isProcessed() const
    {
        return _isProcessed;
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

    // response formatting helpers
    void Response::setStatus(int status)
    {
        _status = HTTP_VERSION " " + SSTR(status) + " " + g_statusMessages.at(status);
    }

    void Response::setHeader(std::string const& name, std::string const& value)
    {
        _header += name + ": " + value + "\r\n";
    }

    void Response::setBody(std::string const& body)
    {
        _body = body;
    }

    void Response::setContentType(std::string const& contentType)
    {
        setHeader("Content-Type", contentType);
    }

    void Response::setContentLength(size_t contentLength)
    {
        setHeader("Content-Length", SSTR(contentLength));
    }

    void Response::setLocation(std::string const& location)
    {
        setHeader("Location", location);
    }

    void Response::setConnection(std::string const& connection)
    {
        setHeader("Connection", connection);
    }

    void Response::setDate()
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, 80, "%a, %d %b %Y %H:%M:%S %Z", timeinfo);
        setHeader("Date", buffer);
    }

    void Response::setResponse(int status, const char* contentType)
    {
        setStatus(status);
        setContentType(contentType);
        setBody(_body);
        setContentLength(_body.size());
    }

    void Response::setResponse(int status, std::string& contentType)
    {
        setStatus(status);
        setContentType(contentType);
        setBody(_body);
        setContentLength(_body.size());
    }

    void Response::setResponse(int status, std::string& contentType, std::string& body)
    {
        setStatus(status);
        setContentType(contentType);
        setBody(body);
        setContentLength(body.size());
    }

    void Response::endResponse()
    {
        if (_isProcessed)
            return;
        setDate();
        setHeader("Server", SERVER_NAME);
        _response = _status + CRLF + _header + CRLF + _body +  CRLF;
        _isProcessed = true;
        console.log("Response formated.");
    }

    int Response::setErrorResponse(int status)
    {
        // use a Set for faster lookup?
        if (g_errorPages.find(status) != g_errorPages.end())
        {
            setStatus(status);
            setContentType("text/html");
            setBody(g_errorPages.at(status));
            setContentLength(_body.size());
            return 0;
        }
        return -1;
    }

    const char *Response::resolveContentType(std::string const & file) const
    {
        const char *type = mimeTypes::getType(file.c_str());
        if (type)
            return type;
        else
            return "text/plain";
    }

    /*
    ** this is called after reading the body of the request
    ** RETURN:
    **  0: file found (sets fileName)
    **  403: found but forbidden
    **  404: file not found
    */
    int Response::resolveIndexFile(
        struct Location const& loc,
        std::string const& path,
        std::string* fileName
    )
    {
        console.warn("Resolving index file...");
        fileName->assign(path); // to check if changed
        t_vec_str::const_iterator it;
        for (it = loc.config.at("index").begin(); it != loc.config.at("index").end(); ++it)
        {
            std::string indexPath;
            if (*path.rbegin() == '/')
                indexPath = path + *it;
            else
                indexPath = path + "/" + *it;
            if (access(indexPath.c_str(), F_OK) == -1)
            {
                std::cout << "=> INDEX: [" << indexPath << "] not found" << std::endl;
                continue;
            }
            if (access(indexPath.c_str(), R_OK) == -1)
            {
                console.warn("[" + indexPath + "] is not readable.");
                return 403; // forbidden
            }
            std::cout << "=> INDEX: [" << indexPath << "] found" << std::endl;
            fileName->assign(indexPath);
            return 0; // index found
        }
        return 404; // index not found
    }

    /* same as above, but doens't set fileName */
    int Response::resolveIndexFile(
        struct Location const& loc,
        std::string const& path
    )
    {
        console.warn("Resolving index file...");
        t_vec_str::const_iterator it;
        for (it = loc.config.at("index").begin(); it != loc.config.at("index").end(); ++it)
        {
            std::string indexPath;
            // if (path.back() == '/')
            if (*path.rbegin() == '/')
                indexPath = path + *it;
            else
                indexPath = path + "/" + *it;
            if (access(indexPath.c_str(), F_OK) == -1)
            {
                std::cout << "=> INDEX: [" << indexPath << "] not found" << std::endl;
                continue;
            }
            if (access(indexPath.c_str(), R_OK) == -1)
            {
                console.warn("[" + indexPath + "] is not readable.");
                return 403; // forbidden
            }
            std::cout << "=> INDEX: [" << indexPath << "] found" << std::endl;
            return 0; // index found
        }
        return 404; // index not found
    }
}
