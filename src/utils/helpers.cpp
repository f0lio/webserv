
#include "Utils.hpp"

bool  is_included(char c, char* str)
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

bool is_number(const char* s)
{
	while (*s)
		if (!std::isdigit(*s++))
			return false;
	return true;
}

bool file_exists(const std::string& name)
{
	return access(name.c_str(), F_OK) != -1;
}

bool is_directory(const std::string& path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
}

bool is_directory(struct stat& st)
{
	return S_ISDIR(st.st_mode);
}

bool is_regular_file(const std::string& path)
{
	struct stat st;
	return stat(path.c_str(), &st) == 0 && S_ISREG(st.st_mode);
}

bool is_regular_file(struct stat& st)
{
	return S_ISREG(st.st_mode);
}

const std::map<int, std::string> initStatusMessages()
{
	std::map<int, std::string> statusMessages;
	statusMessages[200] = "OK";
	statusMessages[201] = "Created";
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

const std::set<std::string> initImplementedMethods()
{
	std::set<std::string> tmp;
	tmp.insert("GET");
	tmp.insert("POST");
	tmp.insert("DELETE");
	return tmp;
}

std::string toUpperStr(std::string const& str)
{
	std::string uppedStr(str);

	for (size_t i = 0; i < uppedStr.size(); i++)
		uppedStr[i] -= 32 * (uppedStr[i] >= 'a' && uppedStr[i] <= 'z');

	return uppedStr;
}

// char to hex string
std::string charToHex(char c)
{
	std::stringstream ss;
	ss << std::hex << (int)c;
	return ss.str();
}

// string to url-encoded string
std::string showWhiteSpaces(std::string const& str)
{
	std::string encodedStr;
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == ' ')
			encodedStr += '+';

		else if (str[i] == '\n')
			encodedStr += "\\n";
		else if (str[i] == '\r')
			encodedStr += "\\r";

		else if (str[i] < ' ' || str[i] > '~')
			encodedStr += "%" + charToHex(str[i]);
		else
			encodedStr += str[i];
	}
	return encodedStr;
}

const std::string& formatDate(time_t time)
{
	struct tm* tm = gmtime(&time);
	char buffer[80];
	strftime(buffer, 80, "%d-%b-%Y %H:%M", tm);
	return buffer;
}

// chwia dial sikiriti
const std::string sanitizeFilename(std::string const& filename)
{
	std::string sanitizedFilename(filename);
	for (size_t i = 0; i < sanitizedFilename.size(); i++)
	{
		if (sanitizedFilename[i] == '.'
		&& i + 1 < sanitizedFilename.size() && sanitizedFilename[i + 1] != '.')
			continue; // dots are allowed, but not consecutive (e.g. extension)
		if (sanitizedFilename[i] == '.'
			|| sanitizedFilename[i] == ' '
			|| sanitizedFilename[i] == '\''
			|| sanitizedFilename[i] == '\"'
			|| sanitizedFilename[i] == '\\'
			|| sanitizedFilename[i] == '/')
			sanitizedFilename[i] = '_';
	}
	return sanitizedFilename;
}

bool isFileNameValid(std::string const& filename)
{
	if (filename.size() == 0 || filename.size() > 255)
		return false;
	else if (filename[0] == '/') // no absolute path
		return false;
	else if (filename[0] == '.') // no hidden files, no . and ..
		return false;
	else if (filename[filename.size() - 1] == '.')
		return false;
	return true;
}

// this is for debugging purposes
#define DBG_COUNTERS_COUNT 10

size_t dbgCounter(size_t index)
{
	static size_t counter[DBG_COUNTERS_COUNT] = { 0 };
	return counter[index]++;
}

const std::string& autoIndex(const std::string& root, const std::string& path)
{
	std::string dir_path = root + path;
	console.log("dir_path: " + dir_path);
	DIR* dir = opendir(dir_path.c_str());
	if (dir)
	{
		struct dirent* ent;
		std::string sufix = "";
		std::string dirContent = \
			"<html><head><title>Index of " + path
			+ "</title></head><body><h1>Index of "
			+ path + "</h1><hr><pre>";
		while ((ent = readdir(dir)) != NULL)
		{
			std::string filePath(ent->d_name);
			if (ent->d_name[0] == '.')
			{
				dirContent += "<a href=\"" + filePath + "\">" + filePath + "</a>\n";
				continue;
			}

			struct stat st;
			stat(filePath.c_str(), &st);

			// if (is_directory(st))
			if (is_directory(filePath))
				sufix = "/";
			else
				sufix = "";
			dirContent += "<a href=\"" + path + "/" + filePath + sufix + "\">" + filePath + "</a>";
			std::string formattedDate = formatDate(st.st_mtime);
			dirContent += "\t\t\t\t\t\t\t" + formattedDate + "\t\t\t\t";

			if (sufix == "/")
				dirContent += "-";
			else
				dirContent += SSTR(st.st_size) + " bytes\n";
			dirContent += "\n";

		}
		dirContent += "</pre><hr></body></html>";
		closedir(dir);
		return dirContent;
	}
	return "AutoIndex: FAILED TO OPEN DIRECTORY";
	// return g_errorPages[403]; // 403 Forbidden
}
