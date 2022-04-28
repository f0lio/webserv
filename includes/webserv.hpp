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
#include "../src/Configuration/VServer.hpp"
#include "../src/Networking/Server.hpp"
// #include "../src/Networking/Cluster.hpp"

// space in between makes it a special key (2 tokens),
//  thus avoid collision with server_name named (e.g. "default_server")")
#define DEFAULT_SERVER_KEY "default server"

typedef unsigned short port_t;
typedef std::vector<std::string> t_vec_str;

struct ServerName
{
    std::map<std::string, ws::VServer *> vservers;
};

struct Location
{
    std::string path;
    std::map<std::string, t_vec_str> config;
};

bool  is_included(char c, char *str);
bool is_number(const std::string &s);
bool is_number(const char *s);

static Console console;