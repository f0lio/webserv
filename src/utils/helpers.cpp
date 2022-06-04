
#include "Utils.hpp"

bool  is_included(char c, char *str)
{
    while (str)
        if (c == *str++)
            return true;
    return false;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool is_number(const char *s)
{
    while (*s)
        if (!std::isdigit(*s++))
            return false;
    return true;
}

const std::map<int, std::string> initStatusMessages()
{
	std::map<int, std::string> statusMessages;
	statusMessages[200] = "OK";
	statusMessages[301] = "Moved Permanently";
	statusMessages[302] = "Found";
	statusMessages[304] = "Not Modified";
	statusMessages[400] = "Bad Request";
	statusMessages[401] = "Unauthorized";
	statusMessages[403] = "Forbidden";
	statusMessages[404] = "Not Found";
	statusMessages[405] = "Method Not Allowed";
	statusMessages[406] = "Not Acceptable";
	statusMessages[407] = "Proxy Authentication Required";
	statusMessages[408] = "Request Timeout";
	statusMessages[409] = "Conflict";
	statusMessages[410] = "Gone";
	statusMessages[411] = "Length Required";
	statusMessages[412] = "Precondition Failed";
	statusMessages[413] = "Request Entity Too Large";
	statusMessages[414] = "Request-URI Too Long";
	statusMessages[415] = "Unsupported Media Type";
	statusMessages[416] = "Requested Range Not Satisfiable";
	statusMessages[417] = "Expectation Failed";
	statusMessages[500] = "Internal Server Error";
	statusMessages[501] = "Not Implemented";
	statusMessages[502] = "Bad Gateway";
	statusMessages[503] = "Service Unavailable";
	statusMessages[504] = "Gateway Timeout";
	statusMessages[505] = "HTTP Version Not Supported";
	return statusMessages;
}

const std::map<int, std::string> initErrorPages()
{
	std::map<int, std::string> errorPages;
	errorPages[301] =
		"<html>" CRLF "<head><title>301 Moved Permanently</title></head>" CRLF
		"<body>" CRLF "<center><h1>301 Moved Permanently</h1></center>" CRLF;

	errorPages[302] =
		"<html>" CRLF "<head><title>302 Found</title></head>" CRLF
		"<body>" CRLF "<center><h1>302 Found</h1></center>" CRLF;

	errorPages[303] =
		"<html>" CRLF "<head><title>303 See Other</title></head>" CRLF
		"<body>" CRLF "<center><h1>303 See Other</h1></center>" CRLF;

	errorPages[307] =
		"<html>" CRLF "<head><title>307 Temporary Redirect</title></head>" CRLF
		"<body>" CRLF "<center><h1>307 Temporary Redirect</h1></center>" CRLF;

	errorPages[308] =
		"<html>" CRLF "<head><title>308 Permanent Redirect</title></head>" CRLF
		"<body>" CRLF "<center><h1>308 Permanent Redirect</h1></center>" CRLF;

	errorPages[400] =
		"<html>" CRLF "<head><title>400 Bad Request</title></head>" CRLF
		"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF;

	errorPages[401] =
		"<html>" CRLF "<head><title>401 Authorization Required</title></head>" CRLF
		"<body>" CRLF "<center><h1>401 Authorization Required</h1></center>" CRLF;

	errorPages[402] =
		"<html>" CRLF "<head><title>402 Payment Required</title></head>" CRLF
		"<body>" CRLF "<center><h1>402 Payment Required</h1></center>" CRLF;

	errorPages[403] =
		"<html>" CRLF "<head><title>403 Forbidden</title></head>" CRLF
		"<body>" CRLF "<center><h1>403 Forbidden</h1></center>" CRLF;

	errorPages[404] =
		"<html>" CRLF "<head><title>404 Not Found</title></head>" CRLF
		"<body>" CRLF "<center><h1>404 Not Found</h1></center>" CRLF;

	errorPages[405] =
		"<html>" CRLF "<head><title>405 Not Allowed</title></head>" CRLF
		"<body>" CRLF "<center><h1>405 Not Allowed</h1></center>" CRLF;

	errorPages[406] =
		"<html>" CRLF "<head><title>406 Not Acceptable</title></head>" CRLF
		"<body>" CRLF "<center><h1>406 Not Acceptable</h1></center>" CRLF;

	errorPages[408] =
		"<html>" CRLF "<head><title>408 Request Time-out</title></head>" CRLF
		"<body>" CRLF "<center><h1>408 Request Time-out</h1></center>" CRLF;

	errorPages[409] =
		"<html>" CRLF "<head><title>409 Conflict</title></head>" CRLF
		"<body>" CRLF "<center><h1>409 Conflict</h1></center>" CRLF;

	errorPages[410] =
		"<html>" CRLF "<head><title>410 Gone</title></head>" CRLF
		"<body>" CRLF "<center><h1>410 Gone</h1></center>" CRLF;

	errorPages[411] =
		"<html>" CRLF "<head><title>411 Length Required</title></head>" CRLF
		"<body>" CRLF "<center><h1>411 Length Required</h1></center>" CRLF;

	errorPages[412] =
		"<html>" CRLF "<head><title>412 Precondition Failed</title></head>" CRLF
		"<body>" CRLF "<center><h1>412 Precondition Failed</h1></center>" CRLF;

	errorPages[413] =
		"<html>" CRLF "<head><title>413 Request Entity Too Large</title></head>" CRLF
		"<body>" CRLF "<center><h1>413 Request Entity Too Large</h1></center>" CRLF;

	errorPages[414] =
		"<html>" CRLF "<head><title>414 Request-URI Too Large</title></head>" CRLF
		"<body>" CRLF "<center><h1>414 Request-URI Too Large</h1></center>" CRLF;

	errorPages[415] =
		"<html>" CRLF "<head><title>415 Unsupported Media Type</title></head>" CRLF
		"<body>" CRLF "<center><h1>415 Unsupported Media Type</h1></center>" CRLF;

	errorPages[416] =
		"<html>" CRLF "<head><title>416 Requested Range Not Satisfiable</title></head>" CRLF
		"<body>" CRLF "<center><h1>416 Requested Range Not Satisfiable</h1></center>" CRLF;

	errorPages[421] =
		"<html>" CRLF "<head><title>421 Misdirected Request</title></head>" CRLF
		"<body>" CRLF "<center><h1>421 Misdirected Request</h1></center>" CRLF;

	errorPages[429] =
		"<html>" CRLF "<head><title>429 Too Many Requests</title></head>" CRLF
		"<body>" CRLF "<center><h1>429 Too Many Requests</h1></center>" CRLF;

	errorPages[494] =
		"<html>" CRLF "<head><title>400 Request Header Or Cookie Too Large</title></head>" CRLF
		"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
		"<center>Request Header Or Cookie Too Large</center>" CRLF;

	errorPages[495] =
		"<html>" CRLF "<head><title>400 The SSL certificate error</title></head>" CRLF
		"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
		"<center>The SSL certificate error</center>" CRLF;

	errorPages[496] =
		"<html>" CRLF "<head><title>400 No required SSL certificate was sent</title></head>" CRLF
		"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
		"<center>No required SSL certificate was sent</center>" CRLF;

	errorPages[497] =
		"<html>" CRLF "<head><title>400 The plain HTTP request was sent to HTTPS port</title></head>" CRLF
		"<body>" CRLF "<center><h1>400 Bad Request</h1></center>" CRLF
		"<center>The plain HTTP request was sent to HTTPS port</center>" CRLF;

	errorPages[500] =
		"<html>" CRLF "<head><title>500 Internal Server Error</title></head>" CRLF
		"<body>" CRLF "<center><h1>500 Internal Server Error</h1></center>" CRLF;

	errorPages[501] =
		"<html>" CRLF "<head><title>501 Not Implemented</title></head>" CRLF
		"<body>" CRLF "<center><h1>501 Not Implemented</h1></center>" CRLF;
	errorPages[502] =
		"<html>" CRLF "<head><title>502 Bad Gateway</title></head>" CRLF
		"<body>" CRLF "<center><h1>502 Bad Gateway</h1></center>" CRLF;

	errorPages[503] =
		"<html>" CRLF "<head><title>503 Service Temporarily Unavailable</title></head>" CRLF
		"<body>" CRLF "<center><h1>503 Service Temporarily Unavailable</h1></center>" CRLF;

	errorPages[504] =
		"<html>" CRLF "<head><title>504 Gateway Time-out</title></head>" CRLF
		"<body>" CRLF "<center><h1>504 Gateway Time-out</h1></center>" CRLF;

	errorPages[505] =
		"<html>" CRLF "<head><title>505 HTTP Version Not Supported</title></head>" CRLF
		"<body>" CRLF "<center><h1>505 HTTP Version Not Supported</h1></center>" CRLF;

	errorPages[507] =
		"<html>" CRLF "<head><title>507 Insufficient Storage</title></head>" CRLF
		"<body>" CRLF "<center><h1>507 Insufficient Storage</h1></center>" CRLF;
	
	return errorPages;
}