
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
			// 	// console.log(_header);

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
				return 414; // Request-URI Too Long
		}

		if (_path.length() > MAX_PATH_LENGTH) // path + query
		{
			console.err("Invalid header: path too long"); 
			return 414; // Request-URI Too Long
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
			console.err("Invalid path: " + _path + " " + _path[_path.find_first_of(PATH_VALID_CHARS)]);
			return 400; // Bad request
		}

		size_t	lineEnd;
		for (size_t i = _header.find('\n') + 1; i < _header.size(); i = lineEnd + 1)
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
		while (true)
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				break;
			buffer[n] = '\0';
			_request.append(buffer, n);
			if (_request.find("\r\n\r\n") != std::string::npos)
				break;
			if (_request.size() > MAX_HEADER_SIZE)
			{
				console.err("Request too long");
				return 431; // Request Header Fields Too Large
			}
		}
		std::string::size_type pos = _request.find("\r\n\r\n");
		if (pos == std::string::npos)
			return 0;
		_header = _request.substr(0, pos);
		_body = _request.substr(pos + 4);

		return processHeader();
	}

	int Request::parseBody()
	{
		char buffer[REQUEST_BUFFER_SIZE];
		if (!_isChunked)
		{
			size_t content_length = atoi(_headers["Content-Length"].c_str());
			while (_body.size() < content_length)
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
			size_t prevSize = 0;

			// an alternative is chunk = _body if !_body.empty()
			while (true)
			{
				if (_body.find("\r\n", prevSize) != std::string::npos)
					break;
				int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				if (n == 0 || n == -1)
					return 0;
				buffer[n] = '\0';

				prevSize = _body.size(); // only search in new bytes ?
				_body.append(buffer, n);

			}

			size_t cl_end = _body.find("\r\n");

			if (_body.find_first_not_of("1234567890") != cl_end)
				return 400;

			size_t chunk_length = atoi(_body.c_str());

			if (chunk_length == 0)
				return 400; // Bad request

			std::string	chunk = _body.substr(cl_end + 2, std::string::npos); // from \r\n to end of buffered _body
			_body.clear(); // _body should only have raw content

			prevSize = 0;
			while (chunk.size() < chunk_length)
			{
				if (chunk.find("\r\n", prevSize) != std::string::npos) // chunk.size() < chunk_length and "\r\n" found
					return 400;
				int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				if (n == 0 || n == -1)
					return 0;
				buffer[n] = '\0';

				prevSize = chunk.size(); // only search in new bytes ?
				chunk.append(buffer, n);
			}
			if (chunk.find("\r\n", prevSize) == std::string::npos) // chunk.size() >= chunk_length and "\r\n" not found
				return 400;
		}
	}

	void Request::process()
	{
		if (this->_status == 0)
			_status = this->parseHeader();
		else if (this->_status == 200)
			_status = this->parseBody();
		console.err("Error: " + SSTR(_status));
		// else go to error page
		this->_isDone = true; // this is wrong!!! change it
	}
} // namespace ws

