#pragma once

#include "webserv.hpp"

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <set>

namespace ws
{

	class Request
	{
	public:
		Request(int client_fd, struct sockaddr_in client_addr);
		~Request();

		std::string const &getHeader() const;
		std::map<std::string, std::string> const &getHeaders() const;
		std::string const &getBody() const;
		std::string const &getMethod() const;
		std::string const &getPath() const;
		std::string const &getQuery() const;
		int const &getFd() const;
		const struct sockaddr_in & getClientAddress() const;

		void process();
		bool isComplete() const;

	private:
		int _fd;
		std::string _request;
		std::string _method;
		std::string _path;
		std::string _query;
		std::string _header;
		std::string _body;
		int			_content_length;
		bool _isHeaderSet;
		bool _isChunked;
		bool _isDone;
		std::map<std::string, std::string> _headers;
		struct sockaddr_in _client_addr;

		//
		void parseHeader();
		void parseBody();
	};

} // namespace ws
