#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h> //printf
#include <netdb.h>

#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "../src/utils/Console.hpp"
#include "../src/utils/Logger.hpp"

#define ARRAY_SIZE(X_X) (sizeof(X_X) / sizeof(X_X[0]))

#define SSTR( x )       static_cast< std::ostringstream & >( \
                        ( std::ostringstream() << std::dec << x  ) ).str()


#define PRINT_TOKEN(TOKEN) printf("%3.zu, %-15s=  [%s]\n",\
                                TOKEN._line,\
                                TokenTypeStrings[TOKEN._type],\
                                TOKEN._value.c_str());

#define PASS 0

#define MAX_PATH_LENGTH 256
#define MAX_HEADER_SIZE 8192
#define REQUEST_BUFFER_SIZE 1024
#define CI_HEX "0123456789abcdefABCDEF"

#define BACK_LOG 10

// space in between makes it a special key (2 tokens), thus avoid collision with server_name named (e.g. "default_server")")
#define DEFAULT_SERVER_KEY "default server"
#define PATH_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="
#define CRLF "\r\n"

bool    is_included(char c, char* str);
bool    is_number(const std::string& s);
bool    is_number(const char* s);

static Console console;

typedef unsigned short port_t;
typedef std::vector<std::string> t_vec_str;

struct Listen
{
    struct sockaddr_in addr_in;
    std::string host;
    port_t port;
    int fd;
};

struct Location
{
    std::string path;
    std::map<std::string, t_vec_str> config;
};
