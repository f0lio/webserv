#pragma once

#include "Utils.hpp"
#include "../src/Configuration/Configuration.hpp"
#include "../src/Networking/Request.hpp"
#include "../src/Networking/Response.hpp"
#include "../src/Networking/Cluster.hpp"
#include "../src/Networking/Constants.hpp"

typedef unsigned short port_t;
typedef std::vector<std::string> t_vec_str;

struct Listen
{
    struct sockaddr_in addr_in;
    std::string host;
    port_t port;
    int fd;
};

struct ServerName
{
    std::map<std::string, ws::VServer*> vservers;
    ws::VServer* get(const std::string& name) const
    {
        std::map<std::string, ws::VServer*>::const_iterator it = vservers.find(name);
        if (it == vservers.end())
            return NULL;
        return it->second;
    }
};

struct Location
{
    std::string path;
    std::map<std::string, t_vec_str> config;
};

bool    is_included(char c, char* str);
bool    is_number(const std::string& s);
bool    is_number(const char* s);

static Console console;
