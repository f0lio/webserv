

#include "Request.hpp"

namespace ws
{
	static char buffer[REQUEST_BUFFER_SIZE];

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

    std::string const& Request::getHeaderField(std::string const &key) const
    {
		if (_headers.find(key) != _headers.end())
			return _headers.at(key);
		throw std::runtime_error("Header field not found");
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
	
    const VServer* Request::resolveVServer() const 
    {
        std::vector<VServer*>::iterator it = _vservers.begin();

        std::string host = _headers.at("Host");
        for (; it != _vservers.end(); ++it)
        {
            if (std::find((*it)->get("server_name").begin(),
                (*it)->get("server_name").end(), host) != (*it)->get("server_name").end())
                return *it;
        }
        return *_vservers.begin();
    }

    bool Request::isComplete() const
    {
        return _isDone;
    }

	int Request::processHeader()
	{
		static const std::string delim = "\r\n"; // potential problems
		{
			size_t	lineEnd = _header.find(delim);
			if (lineEnd == std::string::npos)
				return 400; // Bad request
			
			std::string line = _header.substr(0, lineEnd);
			_header.erase(0, lineEnd + delim.size());

			size_t	space = line.find(' ');
			if (space == std::string::npos)
				return 400; // Bad request
			
			_method = line.substr(0, space);

			if (AllMethods.find(_method) == AllMethods.end())
				return 400; // Bad request

			if (ImplementedMethods.find(_method) == ImplementedMethods.end())
				return 501; // method not implemented
			
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
		for (size_t lineEnd = _header.find(delim, i); i < _header.size(); i = lineEnd + delim.size())
		{
			lineEnd = _header.find(delim, i); // fine if npos
			std::string line = _header.substr(i, lineEnd - i);

			size_t colon = line.find(':');
			if (colon == std::string::npos)
			{
				console.err("Invalid header: " + line);
				return 400; // Bad request
			}
			std::string key = line.substr(0, colon);
			std::string value = line.substr(colon + 1);
			if (key.find(' ') != std::string::npos)
			{
				console.err("Invalid header key: " + key);
				return 400; // Bad request
			}

			value.erase(0, value.find_first_not_of(' '));

			if (_headers.find(key) != _headers.end())
			{
				console.err("Duplicate header: " + key);
				return 400; // Bad request
			}

			_headers[key] = value;

			if (key == "Content-Length")
			{
				_content_length = std::atoi(value.c_str()); // stoi is c++11
				if (value.find_first_not_of("1234567890") != std::string::npos)
				{
					console.err("Invalid content length: " + value);
					return 400; // Bad request
				}
			}
			else if (key == "Transfer-Encoding" && value == "chunked")
			{
				_isChunked = true;

				if (_content_length != -1)
				{
					console.err("Invalid header: Content-Length and Transfer-Encoding");
					return 400; // Bad request
				}
			}

			if (lineEnd == std::string::npos)
				break;
		}
		if (_headers.find("Host") == _headers.end())
		{
			console.err("Invalid header: Host not found");
			return 400; // Bad request
		}

		if (_content_length == -1 && _method == "POST" && !_isChunked)
		{
			return 411; // Length Required
		}

		_vserver = resolveVServer();

		if (std::find(_vserver->get("methods").begin(), _vserver->get("methods").end(), _method) == _vserver->get("methods").end())
			return 405; // Method not allowed

		if (std::find(_vserver->get("max_body_size").begin(), _vserver->get("max_body_size").end(), _method) != _vserver->get("max_body_size").end())
		{
			max_body_size = std::atoi(_vserver->get("max_body_size")[0].c_str()); // **temporary
		}
		
		return _method == "POST" ? READING_BODY : OK_200; // OK
	}

	int Request::parseHeader()
	{
		static const std::string delim = "\r\n\r\n";
		static const size_t potentialMiss = delim.size() - 1;

		size_t prevSize = 0;
		while (true)
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				// std::cout << "\n\nparseHeader+++++++++++++++++ n: " + SSTR(n) + "\n" << std::endl;
			if (n == 0 || n == -1)
			{
				return READING_HEADER;
			}
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
		_body = _request.substr(pos + delim.size());

		return processHeader();
	}

	int Request::chunkedBody() //overcomplicated mess
	{
		static const std::string delim = "\r\n";
		static const size_t potentialMiss = delim.size() - 1;

		_content_length *= (_content_length != -1); 

		size_t prevSize = _content_length;

		while (true)
		{
			if (_body.find(delim, prevSize) != std::string::npos)
				break;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				// std::cout << "\n\nchunkedBody1+++++++++++++++++ n: " + SSTR(n) + "\n" << std::endl;
			if (n == 0 || n == -1) // inf loop if request doesnt have delim
			{
				return READING_BODY;
			}
			buffer[n] = '\0';

			prevSize = _body.size(); // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			_body.append(buffer, n);
		}

		size_t delimPos = _body.find(delim, prevSize);
		size_t rawSize = delimPos - prevSize; // this may be off by one

		size_t cl_start = _body.find_last_not_of(CI_HEX, prevSize, rawSize); //chunk length Start
		// size_t cl_end   = _body.find_first_of (CI_HEX, prevSize, rawSize); //chunk length End

		if (!rawSize || cl_start == delimPos) // no data or no hex
		{
			_content_length = delimPos + delim.size(); // if unexpected structure maybe delim is part of body
			return READING_BODY; // no number
		}

		size_t chunk_length = strtol(_body.c_str() + cl_start, NULL, 16);

		if (chunk_length <= 0) // eof
		{
			if (_body.find(delim + delim, delimPos) != delimPos) // bad delimiter (not \r\n\r\n)
				return 400; // bad request
			else
				return OK_200; // end of body
		}
		std::string	chunk = _body.substr(delimPos + delim.size()); // from after \r\n to end of buffered _body
		_body.erase(delimPos); // _body should only have raw content

		while (chunk.size() < chunk_length)
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1) // inf loop if request doesnt have delim
				break ; //experimenting
			buffer[n] = '\0';

			chunk.append(buffer, n);
		}

		if (chunk.size() >= chunk_length + delim.size()) // debatable
		{
			if (chunk.find(delim, chunk_length) != chunk_length) // non debatable
				return 400;
			chunk.erase(chunk_length, delim.size()); // remove delim
		}
		_body.append(chunk);

		_content_length = chunk_length;
		
		return READING_BODY;
	}

	int Request::parseBody()
	{

		if (!_isChunked)
		{
			while (_body.size() < _content_length)
			{
				int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
					// std::cout << "\n\nparseBody+++++++++++++++++ n: " + SSTR(n) + "\n" << std::endl;
				if (n == 0 || n == -1)
				{
					return READING_BODY;
				}
				buffer[n] = '\0';
				_body.append(buffer, n);
			}
		}
		else
		{
			return chunkedBody();
			// return 400; // chunked WIP
		}
		return OK_200;
	}

	void Request::process()
	{
		// 200 ==> done
		// else error

		if (_status == READING_HEADER)
			_status = parseHeader();
		if (_status == READING_BODY)
			_status = parseBody();
		// else go to error page

		if (_status != OK_200)
			console.err("Error: " + SSTR(_status));
		if (_status < OK_200)
			return;
		_isDone = true; // this is wrong!!! change it
	}
} // namespace ws
