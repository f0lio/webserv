

#pragma once

#include "webserv.hpp"

class Server
{
public:
    Server(const Configuration &config){};
    ~Server(){};

    void start(){
        console.log("Starting server...");
    };
};