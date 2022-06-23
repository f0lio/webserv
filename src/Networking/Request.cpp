#include "Request.hpp"

namespace ws
{
	Request::Request(int client_fd, std::vector<VServer *> &vservers)
		: _client_fd(client_fd), _vservers(vservers)
	{
	}

	Request::~Request()
	{
	}

	std::string const &Request::getHeader() const
	{
		return _header;
	}

	std::string const &Request::getHeaderField(std::string const &key) const
	{
		std::string uppedKey = toUpperStr(key);
		if (_headers.find(uppedKey) != _headers.end())
			return _headers.at(uppedKey);
		return NULL; // this SEGVs in string implicit C-tor // open for ideas
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
		return requestTarget;
	}

	std::string const &Request::getQuery() const
	{
		return _query;
	}

	int const &Request::getClientFd() const
	{
		return _client_fd;
	}
	int const &Request::getStatus() const
	{
		return _status;
	}
	VServer const &Request::getVServer() const
	{
		return *_vserver;
	}

	struct Location const &Request::getLoc() const
	{
		return *_loc;
	}

	std::vector<VServer *> &Request::getVServers() const
	{
		return _vservers;
	}

	const VServer *Request::resolveVServer() const
	{
		std::vector<VServer *>::iterator it = _vservers.begin();

		std::string host = getHeaderField("Host");
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
		return done;
	}

	int Request::requestLineParse()
	{
		size_t lineEnd = _header.find(_delim);

		if (lineEnd == std::string::npos)
			return 400; // Bad request

		std::string requestLine = _header.substr(0, lineEnd);

		_header.erase(0, lineEnd);

		size_t space = requestLine.find(' ');

		if (space > 6)	// sizeof("DELETE")
			return 400; // Bad request

		_method = requestLine.substr(0, space);

		if (ImplementedMethods.find(_method) == ImplementedMethods.end())
		{
			console.err("Unimplemented method: " + _method);
			return 501; // method not implemented
		}

		size_t pathStart = space + 1;
		if (requestLine[pathStart] != '/') // pathStart >= requestLine.size() implicit
			return 400;					   // Bad request

		size_t pathEnd = requestLine.find(' ', pathStart);

		if (pathEnd >= pathStart + MAX_PATH_LENGTH)
		{
			console.err("Path too long: " + requestLine.substr(pathStart, pathEnd - pathStart));
			return 414; // Request-URI Too Long // it is accurate
		}

		requestTarget = requestLine.substr(pathStart, pathEnd - pathStart);

		if (requestTarget.find_first_not_of(PATH_VALID_CHARS) != std::string::npos)
			return 400; // Bad request

		size_t protocolStart = pathEnd + 1;

		if (protocolStart >= requestLine.size())
			return 400; // Bad request

		if (requestLine.compare(protocolStart, 9, "HTTP/1.1"))
			return 400; // Bad request

		size_t const queryStart = requestTarget.find('?');

		if (queryStart != std::string::npos)
		{
			_query = requestTarget.substr(queryStart + 1);
			requestTarget = requestTarget.substr(0, queryStart);
		}
		return 0;
	}

	int Request::processHeader() // https://datatracker.ietf.org/doc/html/rfc822#section-3.1.1 no "folding"
	{
		int ret = requestLineParse();
		if (ret == 400)
			return ret;

		if (requestTarget.find_first_not_of(PATH_VALID_CHARS) != std::string::npos)
		{
			console.err("Invalid path: " + requestTarget + " " + requestTarget[requestTarget.find_first_not_of(PATH_VALID_CHARS)]);
			return 400; // Bad request
		}

		size_t i = 0;
		for (size_t lineEnd = 0; i < _header.size(); i = lineEnd + _delim.size())
		{
			lineEnd = _header.find(_delim, i); // fine if npos

			if (lineEnd == i) // empty line
				continue;

			std::string line = _header.substr(i, lineEnd - i);

			size_t colon = line.find(':');
			if (colon == std::string::npos)
			{
				console.err("Invalid header: " + line);
				return 400; // Bad request
			}
			std::string key = toUpperStr(line.substr(0, colon));
			std::string value = line.substr(colon + 1);
			value.erase(0, value.find_first_not_of(LWSP)); // still more checks needed probably

			if (key.find_first_of(LWSP CRLF) != std::string::npos)
			{
				console.err("Invalid header key: '" + showWhiteSpaces(key) + "'");
				return 400; // Bad request
			}

			if (_headers.find(key) != _headers.end())
			{
				console.err("Duplicate header: " + key);
				return 400; // Bad request
			}

			_headers[key] = value;

			// print key and value
			console.log("Header: " + key + ": " + value);

			if (key == "CONTENT-LENGTH")
			{
				if (value.find_first_not_of("1234567890") != std::string::npos)
				{
					console.err("Invalid content length: " + value);
					return 400; // Bad request
				}
				_content_length = std::atoi(value.c_str()); // stoi is c++11
			}
			else if (key == "TRANSFER-ENCODING")
			{
				if (value == "chunked")
					chunked = true;
				else
				{
					if (!ret)
					{
						console.err("Unsupported transfer encoding: " + value);
						ret = 501; // not implemented
					}
				}
			}

			if (lineEnd == std::string::npos)
				break;
		}
		if (_headers.find("HOST") == _headers.end())
		{
			console.err("Invalid header: Host not found");
			return 400; // Bad request
		}

		if (ret) // first occurring non syntax error
			return ret;

		if (_content_length == -1 && !chunked && _method == "POST")
		{
			console.err("Invalid header: neither Content-Length nor Transfer-Encoding: chunked found with POST method");
			return 411; // Length Required
		}

		if (chunked)
		{
			if (_content_length != -1) // https://www.rfc-editor.org/rfc/rfc9112#section-6.1-14
			{
				console.err("Invalid header: Content-Length and Transfer-Encoding: chunked are mutually exclusive");
				return 400; // Bad request
			}
			_content_length = 0;
		}

		_vserver = resolveVServer();

		if (std::find(_vserver->get("methods").begin(), _vserver->get("methods").end(), _method) == _vserver->get("methods").end())
		{
			console.err("Method not allowed: " + _method);
			return 405; // Method not allowed
		}

		if (std::find(_vserver->get("max_body_size").begin(), _vserver->get("max_body_size").end(), _method) != _vserver->get("max_body_size").end())
		{
			max_body_size = std::atoi(_vserver->get("max_body_size")[0].c_str()); // **temporary
		}

		return OK_200; // OK
	}

	int Request::parseHeader()
	{
		static const size_t potentialMiss = _delim_end.size() - 1;

		size_t prevSize = 0;
		while (true)
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				return READING_HEADER;
			buffer[n] = '\0';
			_request.append(buffer, n);
			if (_request.find(_delim_end, prevSize) != std::string::npos)
				break;
			prevSize = _request.size();
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?

			if (prevSize > MAX_HEADER_SIZE)
			{
				console.err("Request too long");
				return 431; // Request Header Fields Too Large
			}
		}
		size_t pos = _request.find(_delim_end, prevSize); // potential problems

		_header = _request.substr(0, pos);
		_body = _request.substr(pos + _delim_end.size());

		_vserver = *_vservers.begin();

		_status = processHeader();

		setLoc(); // there MUST be checks on when to precheck

		if (_status == OK_200 && _method == "POST")
			_status = READING_BODY;

		return _status;
	}

	int Request::chunkedBody() // overcomplicated mess
	{
		const size_t potentialMiss = _delim.size() - 1;

		size_t prevSize = readIndex;
		prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0;

		while (true)
		{
			if (_body.find(_delim, prevSize) != std::string::npos)
				break;
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
				return READING_BODY;
			buffer[n] = '\0';

			prevSize = _body.size();											 // only search in new bytes ?
			prevSize = prevSize >= potentialMiss ? prevSize - potentialMiss : 0; // if we buffer half delim, we can miss it ?
			_body.append(buffer, n);
		}
		size_t delimPos = _body.find(_delim, _content_length);
		size_t rawSize = delimPos - _content_length;

		size_t cl_start = _body.substr(_content_length, rawSize + 1).find_last_not_of(CI_HEX); // chunk length Start
		cl_start *= (cl_start != rawSize);
		cl_start += _content_length; // it fkn works just have faith

		size_t cl_end = _body.find_first_not_of(CI_HEX, cl_start); // chunk length End

		// print cl_start and cl_end and rawsize and prevSize and delimPos
		// std::cout << "cl_start: " << cl_start << " cl_end: " << cl_end << " rawsize: " << rawSize << " prevSize: " << prevSize << " delimPos: " << delimPos << " _content_length: " << _content_length <<  " readIndex: " << readIndex << std::endl;

		if (!rawSize || cl_end != delimPos) // no data or wrong syntax
		{
			readIndex = delimPos + _delim.size(); // if unexpected structure maybe delim is part of body
			return chunkedBody();				  // kinda risky
		}

		size_t chunk_length = strtol(_body.c_str() + cl_start, NULL, 16);

		std::cout << "chunk_length: " << chunk_length << " - cl_start: " << cl_start << " - cl_end: " << cl_end << " - _body.size(): " << _body.size() << std::endl;

		if (chunk_length == 0) // trailer header not implemented see: https://www.rfc-editor.org/rfc/rfc9112#name-chunked-trailer-section
		{
			if (_body.find(_delim + _delim, delimPos) != delimPos) // bad delimiter (not \r\n\r\n)
			{
				readIndex = delimPos + _delim.size();
				return chunkedBody();
			}
			else
			{
				_body.erase(cl_start);
				return OK_200; // end of body
			}
		}
		std::string chunk = _body.substr(delimPos + _delim.size()); // from after \r\n to end of buffered _body
		_body.erase(cl_start);										// _body should only have raw content

		bool stillReading = 0;

		while (chunk.size() < chunk_length + _delim.size())
		{
			int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
			if (n == 0 || n == -1)
			{
				stillReading = 1;
				break; // experimenting
			}
			buffer[n] = '\0';
			chunk.append(buffer, n);
		}
		// print chunklength
		if (chunk.size() >= chunk_length + _delim.size()) // debatable
		{
			// std::cout << "chunk.find(delim, chunk_length): " << chunk.find(delim, chunk_length) << std::endl;
			if (chunk.find(_delim, chunk_length) != chunk_length) // non debatable
			{
				console.err("Invalid chunk: delim not found? not sure wtf this is");
				return 400;
			}

			chunk.erase(chunk_length, _delim.size()); // remove delim
		}

		_body.append(chunk);

		_content_length += chunk_length;
		readIndex = _content_length;

		if (stillReading) // debatable
			return READING_BODY;

		return chunkedBody();
	}

	int Request::parseBody()
	{
		if (!chunked)
		{
			if (_body.capacity() < _content_length)
				_body.reserve(_content_length);

			while (_body.size() < _content_length)
			{
				int n = read(_client_fd, buffer, REQUEST_BUFFER_SIZE);
				if (n == 0 || n == -1)
					return READING_BODY;
				buffer[n] = '\0';
				_body.append(buffer, n);
			}
			if (!_content_length)
				_body.clear();
		}
		else
			return chunkedBody();
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

		if (_status < OK_200)
			return;
		// if (_status == OK_200)
		// 	std::cout << "SUCCESS _body: '" << showWhiteSpaces(_body) << "'" << std::endl;
		done = true;
	}

	int Request::setLoc() // TODO: WIP: implement functionality(?) in request
	{
		console.warn("Finding VServer...");
		const VServer &vs = getVServer(); // if bad request (400) it is vservers.begin() same as host not found in resolvevserver()

		console.warn("Finding location...");
		_loc = &vs.resolveLocation(requestTarget);

		// print location config
		console.log("Location config: ");
		for (auto const &it : _loc->config)
		{
			console.log("\t", it.first + ":");
			for (auto const &it2 : it.second)
				console.log(" \"", it2, "\"");
			std::cout << std::endl;
		}
		return 0;
	}

} // namespace ws
