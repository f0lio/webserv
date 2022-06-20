#pragma once

#include "VServer.hpp"

namespace ws
{
	class Request
	{
	public:
		Request(int client_fd, std::vector<VServer *> &vservers);
		~Request();

		std::string const &getHeader() const;
		std::string const &getHeaderField(std::string const &key) const;
		std::string const &getBody() const;
		std::string const &getMethod() const;
		std::string const &getPath() const;
		std::string const &getQuery() const;
		VServer const *getVServer() const;
		int const &getStatus() const;
		int const &getClientFd() const;

		std::vector<VServer *> &getVServers() const;
		const VServer *resolveVServer() const;

		void process();
		bool isComplete() const;

	private:
		int _client_fd;
		std::string _request;
		std::string _method;
		std::string requestTarget;
		std::string _query;
		std::string _header;
		std::string _body;
		bool _isChunked = 0;
		bool _isDone = 0;
		size_t _content_length = -1;
		size_t readIndex = 0;
		size_t timeout = 0;
		size_t max_body_size = -1;
		int _status = 0;
		char buffer[REQUEST_BUFFER_SIZE];
		std::map<std::string, std::string> _headers; // capitalise with toUpperStr() before searching, header fields are case-insensitive
		std::vector<VServer *> &_vservers;
		const VServer *_vserver;

		//
		int parseHeader();
		int processHeader();

		int parseBody();
		int chunkedBody();
	};

} // namespace ws
