
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

        const struct Location& loc = _request.getLoc();
        int status = this->precheck(this->_request);

        /*
        **  The following conditional-block gets triggered
        **  when the client GET-requests a specific file (e.g. "GET /image.png HTTP/1.1")
        **  thus, if found we respond with it, otherwise we respond with a 404 error.
        **  When the request's path is a directory, the very last else-block handles it.
        */
	   // this is a duplicate of getRequestHandler()
        // if (status == 200) // success
        // {
        //     console.warn("Precheck passed");
        //     //open file and set body
        //     std::string filePath = loc.config.at("root")[0] + _request.getPath();
        //     console.log("File path: " + filePath);
        //     std::ifstream file;
        //     file.open(filePath.c_str());
        //     if (file.is_open())
        //     {
        //         console.log("File opened");
        //         std::stringstream buffer;
        //         buffer << file.rdbuf();
        //         setBody(buffer.str());
        //         setResponse(status, resolveContentType(filePath));
        //         file.close();
        //     }
        //     else
        //         setErrorResponse(404), console.err("File not found");
        // }
    	if (status == 301) // redirection
        {
            setLocation(loc.config.at("redirect")[0]);
            setStatus(status);
        }
        else if (status == -1) // autoindex is on
        {
            console.log("Autoindexing...");
            setBody(::autoIndex(loc.config.at("root")[0], _request.getPath()));
            setResponse(200, "text/html");

        }
        // else if (status >= 400 && status < 500)
        else if (status) // error
            setErrorResponse(status), console.err("Precheck failed");
        else // status is zero (needs specific handlers)
        {
            if (_request.getMethod() == "POST")
                postRequestHandler();
            else if (_request.getMethod() == "DELETE")
                deleteRequestHandler();
            else if (_request.getMethod() == "HEAD")
                headRequestHandler();
            else if (_request.getMethod() == "GET")
                getRequestHandler();
            // else 
            //     setErrorResponse(405), console.err("Method not allowed");
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
        const struct Location& loc = _request.getLoc();

        if (_request.getStatus() != OK_200)
        {
            std::cout << "_request.getStatus(): " << _request.getStatus() << std::endl;
            return _request.getStatus();
        }

        if (loc.config.find("redirect") != loc.config.end())
            return 301; // redirect to the correct location

        // should use Enums for fast checks? enum {GET, POST, DELETE}
        if (_request.getMethod() == "POST" || _request.getMethod() == "DELETE")
        {
            if (is_directory(loc.config.at("root")[0] + loc.path) == false)
                return 500;
            return 0;
        }

        const std::string& root = loc.config.at("root").at(0);
        const std::string path = root.substr(0, root.find_last_not_of('/') + 1) + req.getPath(); // root does not always ends with '/'

		std::cout << "path: " << path << std::endl;
		std::cout << "root: " << root << std::endl;

        if (file_exists(path) == false)
            return 404;
        else if (is_regular_file(path))
            return 0;

        else if (loc.config.find("index") != loc.config.end())
        {
            std::string fileName;
            console.warn("Inside precheck()");
            int status = resolveIndexFile(loc, path, fileName);
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

    void Response::getRequestHandler()
    {
        console.warn("=> Inside getRequestHandler()");

        const struct Location& loc = _request.getLoc();

        std::string path;
        std::string fileName;

        path = loc.config.at("root")[0] + _request.getPath();

		std::cout << "root: " << loc.config.at("root")[0] << std::endl;
		std::cout << "path: " << path << std::endl;
		std::cout << "_request.getPath(): " << _request.getPath() << std::endl;

		if (!is_regular_file(path))
		{
	        resolveIndexFile(loc, path, fileName);
			path = fileName;
		}
		
        console.log("Opening file: " + path);

        std::ifstream file;
        file.open(path.c_str(), std::ios::in | std::ios::binary);
        if (file.is_open())
        {
            console.log("File opened");
            std::stringstream ss;
            ss << file.rdbuf();
            setBody(ss.str());
            file.close();
            setResponse(200, resolveContentType(path));
        }
        else
            setErrorResponse(404), console.err("[" + path + "] not found");

    }

    void Response::postRequestHandler()
    {
        console.warn("=> Inside postRequestHandler()");
        
        const VServer& vs = _request.getVServer();
        const struct Location& loc = _request.getLoc();

        std::string fileName;
        std::string filePath = loc.config.at("root")[0] + loc.path;
        std::string type = _request.getHeaderField("Content-Type");

        if (filePath.back() != '/')
            filePath += '/';
        
        if (_request.hasHeaderField("filename"))
        {
            if (isFileNameValid(_request.getHeaderField("filename")))
                fileName = ::sanitizeFilename(_request.getHeaderField("filename"));
            else
            {
                setErrorResponse(400), console.err("Invalid file name");
                return;
            }
        }
        else
        {
            const char* ext = mimeTypes::getExtension(type.c_str());
            if (ext == NULL)
            {
                setErrorResponse(415), console.err("Unsupported Media Type");
                return;
            }

            char tmp[] = "/tmp/XXXXXXXX";
            int fd = mkstemp(tmp);

            fileName = tmp;
            fileName += '.';
            fileName += ext;

			if (fd != -1)
			{
				close(fd);
				unlink(tmp);
			}
			else
			{
				std::cout << "tmp was not created: " << tmp << std::endl;
				// maybe doesnt give random string then
			}
        }
        filePath += fileName.substr(5); // skip 
        
        console.log("Saving file: " + filePath);
        
        std::ofstream file;
        file.open(filePath.c_str(), std::ios::out | std::ios::binary);
        if (file.is_open())
        {
            file << _request.getBody();
            file.close();
            console.log("File saved: " + filePath);
            // setHeader("Content-Location", loc.path + fileName);
            setHeader("Location", loc.path + fileName);
            setResponse(201, resolveContentType(filePath));
        }
        else
            setErrorResponse(500), console.err("File not created[" + filePath + "]");
    }

    void Response::deleteRequestHandler()
    {
        console.warn("=> Inside deleteRequestHandler()");
        const struct Location& loc = _request.getLoc();

        std::string path = loc.config.at("root")[0];
        path.pop_back();  // remove the last '/'
        path += _request.getPath();

        if (file_exists(path))
        {
            if (is_regular_file(path))
            {
                if (remove(path.c_str()) == 0)
                    setStatus(204), console.log("File deleted");
                else
                    setErrorResponse(500), console.err("File not deleted");
            }
            else
                setErrorResponse(403), console.err("Not a regular file");
        }
        else
            setErrorResponse(404), console.err("File not found");
    }

    void Response::headRequestHandler()
    {
        console.warn("=> Inside headRequestHandler()");
    }

    void Response::send()
    {
        if (_isSent)
            return;
		int ret = 0;

		while (ret != -1 && !_isSent)
		{
			_sent += ret;
			std::cout << "Response sent: " << convertSize(ret) << " - Total sent: " << convertSize(_sent) << " -  left: " << convertSize(_response.size() - _sent) << std::endl;
	        ret = ::send(_request.getClientFd(), _response.c_str() + _sent, _response.size() - _sent, 0);
			_isSent = _sent == _response.size(); // TODO: need to check if the response is fully sent
		}
        _isProcessed = true;
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
        _bodyIsSet = true;
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
        _response = _status + CRLF + _header + CRLF;
        if (_bodyIsSet)
            _response += _body + CRLF;
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

    const char* Response::resolveContentType(std::string const& file) const
    {
        const char* type = mimeTypes::getType(file.c_str());
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
        std::string &fileName
    )
    {
        console.warn("Resolving index file...");
        fileName.assign(path); // to check if changed
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
            fileName.assign(indexPath);
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
