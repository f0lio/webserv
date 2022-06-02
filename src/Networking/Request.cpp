
#include "Request.hpp"

namespace ws
{

	Request::Request(int client_fd, struct sockaddr_in client_addr)
		: _fd(client_fd)
		, _client_addr(client_addr)
		, _isHeaderSet(false)
		, _isChunked(false)
		, _isDone(false)
	{
	}
	
	Request::~Request()
	{
	}

	
	std::string const &Request::getHeader() const
	{
		return _header;
	}

	std::map<std::string, std::string> const &Request::getHeaders() const
	{
		return _headers;
	}

	std::string const &Request::getBody() const
	{
		return _body;
	}

	std::string const &Request::getMethod() const
	{
		return _method;
	}

	std::string const &Request::getPath() const
	{
		return _path;
	}

	std::string const &Request::getQuery() const
	{
		return _query;
	}

	int const &Request::getFd() const
	{
		return _fd;
	}

	const struct sockaddr_in & Request::getClientAddress() const
	{
		return _client_addr;
	}

	bool Request::isComplete() const
	{
		return _isDone;
	}

	int Request::processHeader()
	{
		static std::set<std::string> methods;
		if (methods.empty())
		{
			methods.insert("GET");
			methods.insert("POST");
			methods.insert("DELETE");
		}
		_method = _header.substr(0, _header.find(' '));
		if (methods.find(_method) == methods.end())
		{
			console.err("Invalid method: " + _method);
			return 1;
		}
		if (_header.find(" HTTP/1.1") == std::string::npos)
		{
			console.err("Invalid header: protocol not supported");
			return 1;
		}
		int pathStart = _header.find(' ') + 1;
		_path = _header.substr(pathStart, _header.find(" H") - pathStart);
		int	queryStart = _path.find('?');
		if (queryStart != std::string::npos)
		{
			_query = _path.substr(queryStart + 1);
			_path = _path.substr(0, queryStart);
		}
		if (_path.find_first_not_of(PATH_VALID_CHARS) != std::string::npos)
		{
			console.err("Invalid path: " + _path);
			return 1;
		}

		for (std::string::size_type i = _header.find('\n') + 1; i < _header.size(); i = _header.find('\n', i) + 1)
		{
			std::string line = _header.substr(i, _header.find('\n', i) - i);
			int colon = line.find(':');
			if (colon == std::string::npos)
			{
				console.err("Invalid header: " + line);
				return 1;
			}
			std::string key = line.substr(0, colon);
			std::string value = line.substr(colon + 2);
			_headers[key] = value;

			if (key == "Content-Length")
			{
				_content_length = std::stoi(value);
			}
			else if (key == "Transfer-Encoding" && value == "chunked")
			{
				_isChunked = true;
			}

			if (_header.find('\n', i) == std::string::npos)
				break;
		}
		return 0;
	}

	void Request::parseHeader()
	{
		char buffer[REQUEST_BUFFER_SIZE];
		// console.log("Parsing header");
		while (true)
		{
			int n = read(_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				break;
			buffer[n] = '\0';
			_request.append(buffer, n);
			if (_request.find("\r\n\r\n") != std::string::npos)
				break;
		}
		usleep(100);
		std::string::size_type pos = _request.find("\r\n\r\n");
		if (pos == std::string::npos)
			return;
		_header = _request.substr(0, pos);
		_body = _request.substr(pos + 4);
		_isHeaderSet = true;

		if (processHeader())
			return;
		// console.log("Header is set");
		usleep(100);
	}

	void Request::parseBody()
	{
		// if (_headers.find("Content-Length") == _headers.end())
		//	 return;
		// else if (checkHeaderDirective("Content-Length", _headers["Content-Length"])
		// {
		//	 _body = _request.substr(pos + 4);
		// }
		// console.log("Parsing body");
		char buffer[REQUEST_BUFFER_SIZE];
		size_t content_length = atoi(_headers["Content-Length"].c_str());
		while (_body.size() < content_length)
		{
			int n = read(_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				break;
			buffer[n] = '\0';
			_body.append(buffer, n);
		}
		usleep(100);
	}

	void Request::process()
	{
		if (this->_isHeaderSet == false)
			this->parseHeader();
		else
			this->parseBody();
		// console.log("### processed! ###");
		this->_isDone = true;
	}
} // namespace ws
