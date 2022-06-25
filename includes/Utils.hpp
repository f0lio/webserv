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
#include <sys/stat.h>
#include <dirent.h>

#include "../src/utils/Console.hpp"
#include "../src/utils/Logger.hpp"
#include "../src/utils/mimeTypes.hpp"
#include "../src/Configuration/ParserRules.hpp"

#define ARRAY_SIZE(X_X) (sizeof(X_X) / sizeof(X_X[0]))

#define SSTR(x) static_cast<std::ostringstream &>(           \
					(std::ostringstream() << std::dec << x)) \
					.str()

#define PRINT_TOKEN(TOKEN) printf("%3.zu, %-15s=  [%s]\n",       \
								  TOKEN._line,                   \
								  TokenTypeStrings[TOKEN._type], \
								  TOKEN._value.c_str());

#define PASS 0

// for request
#define READING_HEADER 0
#define READING_BODY 1
#define OK_200 200

#define MAX_PATH_LENGTH 8192 // RFC compliant see: https://www.rfc-editor.org/rfc/rfc9112#section-3-5
#define MAX_HEADER_SIZE 8192
#define REQUEST_BUFFER_SIZE 1024 // may break some shit now
#define CI_HEX "0123456789abcdefABCDEF"

#define BACK_LOG 128

// space in between makes it a special key (2 tokens), thus avoid collision with server_name named (e.g. "default_server")")
#define DEFAULT_SERVER_KEY "default server"
#define PATH_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%"
#define CRLF "\r\n"
#define LWSP " \t"

static Console console;

const std::map<int, std::string> initStatusMessages();
const std::map<int, std::string> initErrorPages();

const std::set<std::string> initImplementedMethods();

static const std::map<int, std::string>
	g_statusMessages = initStatusMessages();

static const std::map<int, std::string>
	g_errorPages = initErrorPages();

// no need for all methods as per RFC see: https://www.rfc-editor.org/rfc/rfc9112#section-3-4

static const std::set<std::string>
	ImplementedMethods = initImplementedMethods();

static const std::map<std::string, DirectiveRules>
	directiveRulesMap = initDirectiveRules();

static const std::map<std::string, DirectiveRules>
	locationDirectiveRulesMap = initLocationDirectiveRules();

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

// function prototypes
bool is_included(char c, char *str);
bool is_number(const std::string &s);
bool is_number(const char *s);

std::string toUpperStr(std::string const &str);
std::string showWhiteSpaces(std::string const &str);

const std::string &autoIndex(
	const std::string &root, const std::string &path);

bool file_exists(const std::string &name);
bool file_exists(const std::string &path, struct stat &st);

bool is_directory(const std::string &path);
bool is_directory(struct stat &st);

bool is_regular_file(const std::string &path);
bool is_regular_file(struct stat &st);

bool is_readable_file(const std::string &path);
bool is_readable_file(struct stat &st);

bool is_executable_file(const std::string &path);
bool is_executable_file(struct stat &st);
