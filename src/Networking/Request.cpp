
#include "Request.hpp"

namespace ws
{

    Request::Request(int client_fd, std::vector<VServer*> & vservers)
        : _client_fd(client_fd)
        , _vservers(vservers)
    {
    }

    Request::~Request()
    {
    }


    std::string const& Request::getHeader() const
    {
        return _header;
    }

    std::string const& Request::getBody() const
    {
        return _body;
    }

    std::string const& Request::getMethod() const
    {
        return _method;
    }

    std::string const& Request::getPath() const
    {
        return _path;
    }

    std::string const& Request::getQuery() const
    {
        return _query;
    }

    int const& Request::getClientFd() const
    {
        return _client_fd;
    }
    int const& Request::getStatus() const
    {
        return _status;
    }

    std::vector<VServer*> & Request::getVServers() const
    {
        return _vservers;
    }

    bool Request::isComplete() const
    {
        return _isDone;
    }

	int Request::processHeader()
	{
		{
			// size_t const pathStart = _header.find(' ') + 1;
			
			// {

			// 	if (!(pathStart - 1) || ((pathStart - 1) == std::string::npos))
			// 		return 400; // Bad Request

			// 	_method = _header.substr(0, pathStart - 1);
			// 	if (methods.find(_method) == methods.end())
			// 	{
			// 		console.err("Invalid method: " + _method);
			// 		return 400; // Bad request
			// 	}
			// }

			// if (_header.find(" HTTP/1.1") == std::string::npos)
			// {
			// 	console.err("Invalid header: protocol not supported");
			// 	return 400; // Bad request
			// }

			// _path = _header.substr(pathStart, _header.find(" H") - pathStart);

			// std::cout << "\n" << "\n" << _header << "\n" << "\n";
			static std::set<std::string> methods;
			if (methods.empty())
			{
				methods.insert("GET");
				methods.insert("POST");
				methods.insert("DELETE");
			}
			size_t	lineEnd = _header.find('\n');
			if (lineEnd == std::string::npos)
				return 400; // Bad request
			
			std::string line = _header.substr(0, lineEnd);

			size_t	space = line.find(' ');
			if (space == std::string::npos)
				return 400; // Bad request
			
			_method = line.substr(0, space);

			if (methods.find(_method) == methods.end())
				return 400; // Bad request
			
			size_t	pathStart = line.find_first_not_of(' ', space + 1);
			if (pathStart == std::string::npos || line[pathStart] != '/')
				return 400; // Bad request

			size_t	pathEnd = line.find(' ', pathStart);
			if (pathEnd == std::string::npos)
				return 400; // Bad request

			if (pathEnd - pathStart > MAX_PATH_LENGTH)
				return 414; // Request-URI Too Long //prob not accurate path is not URI
			
			_path = line.substr(pathStart, pathEnd - pathStart);
		}

		{
			size_t const queryStart = _path.find('?');

			if (queryStart != std::string::npos)
			{
				_query = _path.substr(queryStart + 1);
				_path = _path.substr(0, queryStart);
			}
		}

		if (_path.find_first_not_of(PATH_VALID_CHARS) != std::string::npos)
		{
			console.err("Invalid path: " + _path + " " + _path[_path.find_first_not_of(PATH_VALID_CHARS)]);
			return 400; // Bad request
		}

		size_t	i = 0;
		for (size_t lineEnd = _header.find('\n', i); i < _header.size(); i = lineEnd + 1)
		{
			lineEnd = _header.find('\n', i); // fine if npos
			std::string line = _header.substr(i, lineEnd - i);

			size_t colon = line.find(':');
			if (colon == std::string::npos)
			{
				console.err("Invalid header: " + line);
				return 400; // Bad request
			}
			std::string key = line.substr(0, colon);
			std::string value = line.substr(colon + 2);

			_headers[key] = value;

			if (key == "Content-Length")
			{
				_content_length = std::atoi(value.c_str()); // stoi is c++11
				if (line.find_first_not_of("1234567890", colon) != std::string::npos)
					return 400; // Bad request
			}
			else if (key == "Transfer-Encoding" && value == "chunked")
			{
				_isChunked = true;
			}

			if (lineEnd == std::string::npos)
				break;
		}
		return 200; // OK
	}

	int Request::parseHeader()
	{
		char buffer[REQUEST_BUFFER_SIZE];

		static const std::string delim = "\r\n\r\n";
		static const size_t potentialMiss = delim.size() - 1;

		size_t prevSize = 0;
		while (true)
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				return 0;
			buffer[n] = '\0';
			_request.append(buffer, n);
			if (_request.find(delim, prevSize) != std::string::npos)
				break;
			prevSize = _request.size();
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?

			if (prevSize > MAX_HEADER_SIZE)
			{
				console.err("Request too long");
				return 431; // Request Header Fields Too Large
			}
		}
		size_t pos = _request.find(delim, prevSize);

		_header = _request.substr(0, pos);
		_body = _request.substr(pos + 4);

		_status = processHeader();
		return _status;
	}

	int Request::chunkedBody() //overcomplicated mess
	{
		char buffer[REQUEST_BUFFER_SIZE];

		static const std::string delim = "\r\n";
		static const size_t potentialMiss = delim.size() - 1;

		size_t prevSize = 0;

		// an alternative is chunk = _body if !_body.empty()
		while (true)
		{
			if (_body.find(delim, prevSize) != std::string::npos)
				break;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				return 0;
			buffer[n] = '\0';

			prevSize = _body.size(); // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			_body.append(buffer, n);
		}

		size_t cl_end = _body.find(delim, prevSize);

		if (_body.find_first_not_of(CI_HEX) != cl_end)
			return 400;

		size_t chunk_length = strtol(_body.c_str(), NULL, 16);

		if (chunk_length == 0) // or is eof TODO
			return 400; // Bad request

		std::string	chunk = _body.substr(cl_end + 2); // from after \r\n to end of buffered _body
		_body.erase(cl_end); // _body should only have raw content

		prevSize = 0;
		while (chunk.size() < chunk_length)
		{
			if (chunk.find(delim, prevSize) != std::string::npos) // chunk.size() < chunk_length and delim found
				return 400;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				break ; //experimenting
			buffer[n] = '\0';

			prevSize = chunk.size(); // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			chunk.append(buffer, n);
		}
		if (chunk.size() >= chunk_length)
		{
			if (chunk.find(delim, prevSize) != chunk_length) // chunk.size() > chunk_length and delim found || delim not found
				return 400;
		}
		// else read() break and everything fine 
		_body += chunk;
	}

	int Request::parseBody()
	{
		char buffer[REQUEST_BUFFER_SIZE];
		if (!_isChunked)
		{
			while (_content_length < 0 || _body.size() < _content_length)
			{
				int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				if (n == 0 || n == -1)
					return 0;
				buffer[n] = '\0';
				_body.append(buffer, n);
			}
		}
		else
		{
			return chunkedBody();
		}
	}

	void Request::process()
	{
		if (this->_status == 0)
			_status = this->parseHeader();
		else if (this->_status == 200)
			_status = this->parseBody();
		// else go to error page

		if (_status && _status != 200)
			console.err("Error: " + SSTR(_status));

		this->_isDone = true; // this is wrong!!! change it
	}
} // namespace ws

