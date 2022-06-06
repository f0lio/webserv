#pragma once

#include "Request.hpp"

namespace ws
{
    class Configuration;
    class Request;
    class Response
    {
    public:
        Response(Request const &request, const Configuration &config);
        ~Response();

        std::string const &getBody() const;
        std::string const &getHeader() const;
        std::string const &getStatus() const;
        
        int precheck(Request const& request);
        void setup();
        void process();
        void send();
        bool isProcessed() const;
        bool isSent() const;

    private:
        Request const &_request;
        std::string _response;
        std::string _header;
        std::string _body;
        std::string _status;
        bool _isProcessed;
        bool _isSent;
        const Configuration &_config;
    };
} // namespace ws
