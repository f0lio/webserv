#pragma once

#include "webserv.hpp"

#define MAX_PATH_LENGTH 256
#define MAX_HEADER_SIZE 8192
#define REQUEST_BUFFER_SIZE 1024
#define CI_HEX "0123456789abcdefABCDEF"

#define BACK_LOG 10

// space in between makes it a special key (2 tokens), thus avoid collision with server_name named (e.g. "default_server")")
#define DEFAULT_SERVER_KEY "default server"
#define PATH_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;="
#define CRLF "\r\n"

namespace ws
{
	static std::map<int, std::string> g_statusMessages;
	static std::map<int, std::string> g_errorPages;

	static void initStatusMessages()
	{
		g_statusMessages[200] = "OK";
		g_statusMessages[301] = "Moved Permanently";
		g_statusMessages[302] = "Found";
		g_statusMessages[304] = "Not Modified";
		g_statusMessages[400] = "Bad Request";
		g_statusMessages[401] = "Unauthorized";
		g_statusMessages[403] = "Forbidden";
		g_statusMessages[404] = "Not Found";
		g_statusMessages[405] = "Method Not Allowed";
		g_statusMessages[406] = "Not Acceptable";
		g_statusMessages[407] = "Proxy Authentication Required";
		g_statusMessages[408] = "Request Timeout";
		g_statusMessages[409] = "Conflict";
		g_statusMessages[410] = "Gone";
		g_statusMessages[411] = "Length Required";
		g_statusMessages[412] = "Precondition Failed";
		g_statusMessages[413] = "Request Entity Too Large";
		g_statusMessages[414] = "Request-URI Too Long";
		g_statusMessages[415] = "Unsupported Media Type";
		g_statusMessages[416] = "Requested Range Not Satisfiable";
		g_statusMessages[417] = "Expectation Failed";
		g_statusMessages[500] = "Internal Server Error";
		g_statusMessages[501] = "Not Implemented";
		g_statusMessages[502] = "Bad Gateway";
		g_statusMessages[503] = "Service Unavailable";
		g_statusMessages[504] = "Gateway Timeout";
		g_statusMessages[505] = "HTTP Version Not Supported";
	}

	static void initErrorPages()
	{
		g_errorPages[301] =
			"<html>" CRLF "<head><title>301 Moved Permanently</title></head>" CRLF
			"<body>" CRLF "<center><h1>301 Moved Permanently</h1></center>" CRLF;

		g_errorPages[302] =
			"<html>" CRLF "<head><title>302 Found</title></head>" CRLF
			"<body>" CRLF "<center><h1>302 Found</h1></center>" CRLF;

		g_errorPages[303] =
			"<html>" CRLF "<head><title>303 See Other</title></head>" CRLF
			"<body>" CRLF "<center><h1>303 See Other</h1></center>" CRLF;

		g_errorPages[307] =
			"<html>" CRLF "<head><title>307 Temporary Redirect</title></head>" CRLF
			"<body>" CRLF "<center><h1>307 Temporary Redirect</h1></center>" CRLF;

		g_errorPages[308] =
			"<html>" CRLF "<head><title>308 Permanent Redirect</title></head>" CRLF
			"<body>" CRLF "<center><h1>308 Permanent Redirect</h1></center>" CRLF;

		g_errorPages[400] =
			"<html>" CRLF "<head><title>400 Bad Request</title></head>" CRLF
			"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF;

		g_errorPages[401] =
			"<html>" CRLF "<head><title>401 Authorization Required</title></head>" CRLF
			"<body>" CRLF "<center><h1>401 Authorization Required</h1></center>" CRLF;

		g_errorPages[402] =
			"<html>" CRLF "<head><title>402 Payment Required</title></head>" CRLF
			"<body>" CRLF "<center><h1>402 Payment Required</h1></center>" CRLF;

		g_errorPages[403] =
			"<html>" CRLF "<head><title>403 Forbidden</title></head>" CRLF
			"<body>" CRLF "<center><h1>403 Forbidden</h1></center>" CRLF;

		g_errorPages[404] =
			"<html>" CRLF "<head><title>404 Not Found</title></head>" CRLF
			"<body>" CRLF "<center><h1>404 Not Found</h1></center>" CRLF;

		g_errorPages[405] =
			"<html>" CRLF "<head><title>405 Not Allowed</title></head>" CRLF
			"<body>" CRLF "<center><h1>405 Not Allowed</h1></center>" CRLF;

		g_errorPages[406] =
			"<html>" CRLF "<head><title>406 Not Acceptable</title></head>" CRLF
			"<body>" CRLF "<center><h1>406 Not Acceptable</h1></center>" CRLF;

		g_errorPages[408] =
			"<html>" CRLF "<head><title>408 Request Time-out</title></head>" CRLF
			"<body>" CRLF "<center><h1>408 Request Time-out</h1></center>" CRLF;

		g_errorPages[409] =
			"<html>" CRLF "<head><title>409 Conflict</title></head>" CRLF
			"<body>" CRLF "<center><h1>409 Conflict</h1></center>" CRLF;

		g_errorPages[410] =
			"<html>" CRLF "<head><title>410 Gone</title></head>" CRLF
			"<body>" CRLF "<center><h1>410 Gone</h1></center>" CRLF;

		g_errorPages[411] =
			"<html>" CRLF "<head><title>411 Length Required</title></head>" CRLF
			"<body>" CRLF "<center><h1>411 Length Required</h1></center>" CRLF;

		g_errorPages[412] =
			"<html>" CRLF "<head><title>412 Precondition Failed</title></head>" CRLF
			"<body>" CRLF "<center><h1>412 Precondition Failed</h1></center>" CRLF;

		g_errorPages[413] =
			"<html>" CRLF "<head><title>413 Request Entity Too Large</title></head>" CRLF
			"<body>" CRLF "<center><h1>413 Request Entity Too Large</h1></center>" CRLF;

		g_errorPages[414] =
			"<html>" CRLF "<head><title>414 Request-URI Too Large</title></head>" CRLF
			"<body>" CRLF "<center><h1>414 Request-URI Too Large</h1></center>" CRLF;

		g_errorPages[415] =
			"<html>" CRLF "<head><title>415 Unsupported Media Type</title></head>" CRLF
			"<body>" CRLF "<center><h1>415 Unsupported Media Type</h1></center>" CRLF;

		g_errorPages[416] =
			"<html>" CRLF "<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
			"<body>" CRLF "<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF;

		g_errorPages[421] =
			"<html>" CRLF "<head><title>421 Misdirected Request</title></head>" CRLF
			"<body>" CRLF "<center><h1>421 Misdirected Request</h1></center>" CRLF;

		g_errorPages[429] =
			"<html>" CRLF "<head><title>429 Too Many Requests</title></head>" CRLF
			"<body>" CRLF "<center><h1>429 Too Many Requests</h1></center>" CRLF;

		g_errorPages[494] =
			"<html>" CRLF "<head><title>400 Request Header Or Cookie Too Large</title></head>" CRLF
			"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>Request Header Or Cookie Too Large</center>" CRLF;

		g_errorPages[495] =
			"<html>" CRLF "<head><title>400 The SSL certificate error</title></head>" CRLF
			"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>The SSL certificate error</center>" CRLF;

		g_errorPages[496] =
			"<html>" CRLF "<head><title>400 No required SSL certificate was sent</title></head>" CRLF
			"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>No required SSL certificate was sent</center>" CRLF;

		g_errorPages[497] =
			"<html>" CRLF "<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>" CRLF
			"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
			"<center>The plain HTTP request was sent to HTTPS port</center>" CRLF;

		g_errorPages[500] =
			"<html>" CRLF "<head><title>500 Internal Server Error</title></head>" CRLF
			"<body>" CRLF "<center><h1>500 Internal Server Error</h1></center>" CRLF;

		g_errorPages[501] =
			"<html>" CRLF "<head><title>501 Not Implemented</title></head>" CRLF
			"<body>" CRLF "<center><h1>501 Not Implemented</h1></center>" CRLF;
		g_errorPages[502] =
			"<html>" CRLF "<head><title>502 Bad Gateway</title></head>" CRLF
			"<body>" CRLF "<center><h1>502 Bad Gateway</h1></center>" CRLF;

		g_errorPages[503] =
			"<html>" CRLF "<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
			"<body>" CRLF "<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF;

		g_errorPages[504] =
			"<html>" CRLF "<head><title>504 Gateway Time-out</title></head>" CRLF
			"<body>" CRLF "<center><h1>504 Gateway Time-out</h1></center>" CRLF;

		g_errorPages[505] =
			"<html>" CRLF "<head><title>505 HTTP Version Not Supported</title></head>" CRLF
			"<body>" CRLF "<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF;

		g_errorPages[507] =
			"<html>" CRLF "<head><title>507 Insufficient Storage</title></head>" CRLF
			"<body>" CRLF "<center><h1>507 Insufficient Storage</h1></center>" CRLF;
	}
} // namespace ws
