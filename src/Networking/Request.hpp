#pragma once

#include "VServer.hpp"

namespace ws
{
	class Request
	{
	public:
		Request(int client_fd, std::vector<VServer*>& vservers);
		~Request();

		std::string const& getHeader() const;
		std::map<std::string, std::string> const& getHeaders() const;
		std::string const& getHeaderField(std::string const& key) const;
		bool hasHeaderField(std::string const& key) const;
		std::string const& getBody() const;
		std::string const& getMethod() const;
		std::string const getPath() const;
		std::string const& getQuery() const;
		VServer const& getVServer() const;
		struct Location const& getLoc() const;
		int const& getStatus() const;
		int const& getClientFd() const;

		std::vector<VServer*>& getVServers() const;

		void process();
		bool isComplete() const;

	private:
		int _client_fd;
		std::string _request;
		std::string _method;
		std::string requestTarget; // pseudo-TODO: https://www.rfc-editor.org/rfc/rfc3986#section-2.1 // https://www.rfc-editor.org/rfc/rfc2396#section-2
		std::string _query;
		std::string _header;
		std::string _body;
		bool chunked;
		bool done;
		size_t _content_length;
		bool _has_content_length;
		size_t readIndex;
		size_t timeout;
		size_t max_body_size;
		int _status;
		char buffer[REQUEST_BUFFER_SIZE];
		std::map<std::string, std::string> _headers; // capitalise with toUpperStr() before searching, header fields are case-insensitive
		std::vector<VServer*>& _vservers;
		const VServer* _vserver;
		const struct Location* _loc;

		const std::string _delim;
		const std::string _delim_end;

		int setLoc();
		int parseHeader();
		int processHeader();
		int requestLineParse();

		int parseBody();
		int chunkedBody();

		const VServer* resolveVServer() const;
	};

} // namespace ws
