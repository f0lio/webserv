#pragma once

#include <iostream>
#include <sstream>
#include "Request.hpp"

namespace ws
{
    class Request;
    class Response
    {
    public:
        Response(Request const &request);
        ~Response();

        std::string const &getBody() const;
        std::string const &getHeader() const;
        std::string const &getStatus() const;
        
        void setup();
        void process();

    private:
        Request const &_request;
        std::string _response;
        std::string _header;
        std::string _body;
        std::string _status;
    };
} // namespace ws
