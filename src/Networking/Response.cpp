
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

        int status = this->precheck(this->_request);

        if (status == 301)
        {
            const VServer& vs = *_request.resolveVServer();
            const struct Location& loc = vs.resolveLocation(_request.getPath());

            this->setStatus(status);
            setBody(g_errorPages.at(status));
            setLocation(loc.config.at("redirect")[0]);
            setContentType("text/html");
            setContentLength(_body.size());

        }
        else if (status == -1) // autoindex is on
        {
            const VServer& vs = *_request.resolveVServer();
            const struct Location& loc = vs.resolveLocation(_request.getPath());

            console.log("Autoindexing...");

            setStatus(200);
            setContentType("text/html");
            setBody(::autoIndex(loc.config.at("root")[0], _request.getPath()));
            setContentLength(_body.size());
        }
        else if (status)
        {
            console.err("Precheck failed");
            setStatus(status);
            setHeader("Content-Type", "text/html");
            setBody(g_errorPages.at(status));
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

            std::string fileName;
            resolveFile(loc, path, &fileName);
            path = fileName;
            console.log("Opening file: " + path);

            std::ifstream file(path, std::ios::in | std::ios::binary);
            if (file.is_open())
            {
                console.log("File opened");
                std::stringstream ss;
                ss << file.rdbuf();
                _body = ss.str();
                file.close();
                try
                {
                    setStatus(200);
                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }

                setContentType("text/html");
                setContentLength(_body.size());
            }
            else
            {
                console.err("[" + path + "] not found");
                try
                {
                    console.err("[" + path + "] not found");
                    setStatus(404);

                }
                catch (const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                }
                setHeader("Content-Type", "text/html");
                setBody(g_errorPages.at(404));
            }
        }

        setResponse();
        _isProcessed = true;
        console.log("Response formated.");
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

        console.warn("Finding VServer...");
        const VServer& vs = *_request.resolveVServer();

        console.warn("Finding location...");
        const struct Location& loc = vs.resolveLocation(_request.getPath());

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

        try
        {
            console.log("=> LOCATION ROOT: [" + loc.config.at("root")[0] + "]");
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        const std::string& root = loc.config.at("root")[0];
        const std::string path = root + req.getPath();

        console.log("=> ROOT: [" + root + "]");
        console.log("=> PATH: [" + path + "]");

        // check if directory exists
        if (access(path.c_str(), F_OK) == -1)
        {
            console.warn("[" + path + "] not found");
            return 404;
        }

        if (loc.config.find("index") != loc.config.end())
        {
            std::string fileName;
            int status = resolveFile(loc, path, &fileName);
            if (status == 0)
            {
                console.err("[" + fileName + "] FOUND?");
                return 0;
            }   
        }

        struct stat st;
        stat(path.c_str(), &st);
        if (S_ISDIR(st.st_mode))
        {
            console.warn("[" + path + "] is a directory");
            if (loc.config.find("autoindex") != loc.config.end())
            {
                console.warn("checking if autoindex is on");
                if (loc.config.at("autoindex")[0] == "on")
                    return -1; // autoindex is on
            }
            return -1;
        }
        return 404; // not found
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

    void Response::setResponse()
    {
        _response = _status + CRLF + _header + CRLF + CRLF + _body;
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

    int Response::resolveFile(struct Location const& loc,
        std::string const& path,
        std::string* fileName
    )
    {
        console.log("Resolving file...");
        fileName->assign(path); // to check if changed
        t_vec_str::const_iterator it;
        for (it = loc.config.at("index").begin(); it != loc.config.at("index").end(); ++it)
        {
            std::string indexPath;
            if (path.back() == '/')
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
}
