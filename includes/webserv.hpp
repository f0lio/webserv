#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "Utils.hpp"
#include "../src/utils/Console.hpp"
#include "../src/utils/Logger.hpp"
#include "../src/Configuration/Configuration.hpp"
#include "../src/Configuration/Directive.hpp"
#include "../src/Configuration/Context.hpp"
#include "../src/Networking/VServer.hpp"
#include "../src/Networking/Request.hpp"
#include "../src/Networking/Response.hpp"
#include "../src/Networking/Cluster.hpp"

// space in between makes it a special key (2 tokens),
//  thus avoid collision with server_name named (e.g. "default_server")")
#define DEFAULT_SERVER_KEY "default server"
#define BACK_LOG 10
#define REQUEST_BUFFER_SIZE 1024
#define PATH_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="
#define CI_HEX "1234567890abcdefABCDEF"
#define MAX_PATH_LENGTH 256
#define MAX_HEADER_SIZE 8192

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
