#pragma once

#include "Request.hpp"
#include "../CGI/CGI.hpp"

#define HTTP_VERSION "HTTP/1.1"
#define SERVER_NAME "Shabalido"

namespace ws
{
    class Configuration;
    class Request;
    class Response
    {
    public:
        Response(Request const& request);
        ~Response();

        std::string const& getBody() const;
        std::string const& getHeader() const;
        std::string const& getStatus() const;

        void setup();
        void process();
        void send();
        bool isProcessed() const;
        bool isSent() const;

    private:
        Request const& _request;
        const VServer& _vs;
        const Location& _loc;
        std::string _response;
        std::string _header;
        std::string _body;
        bool        _bodyIsSet;
        std::string _status;
        bool _isProcessed;
        bool _isSent;
        size_t _sent;

        // private methods
        int precheck(Request const& request); // to be moved to request class

        void setStatus(int status);
        void setHeader(std::string const& name, std::string const& value);
        void setBody(std::string const& body);
        void setContentType(std::string const& contentType);
        void setContentLength(size_t contentLength);
        void setLocation(std::string const& location);
        void setConnection(std::string const& connection);
        void setDate();

        void setResponse(int status, const char* contentType); // tmp
        void setResponse(int status, std::string& contentType);
        void setResponse(int status, std::string& contentType, std::string& body);
        int setErrorResponse(int status);
        int setDefaultErrorPage(int status);
        void endResponse();

        void getRequestHandler();
        void postRequestHandler();
        void deleteRequestHandler();
        void headRequestHandler();
        void cgiHandler();

        bool isAutoIndexEnabled() const;
        const char* resolveContentType(std::string const& file) const;
        //tmp
        int resolveIndexFile(
            struct Location const& loc,
            std::string const& path,
            std::string& fileName);

        int resolveIndexFile(
            struct Location const& loc,
            std::string const& path);

    };
} // namespace ws
