#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include "../Configuration/Configuration.hpp"

#ifdef WITH_KQUEUE
#include "../EventMonitors/KQueue.hpp"
#else
#include "../EventMonitors/Poll.hpp"
#endif

#define MAX_FDS 1024

namespace ws
{
	class Configuration;
	class Response;
	class Request;
	class Cluster
	{
	public:
		Cluster(Configuration const& config);
		~Cluster();

		void setup();
		void run();

	private:

#ifdef WITH_KQUEUE
		KQueue               _io;
#else
		Poll                 _io;
#endif

		Configuration const& _config;
		bool                        _running;
		bool                        _setup;

		std::set<int>               _server_fds;
		std::vector<struct Listen>  _binded_listens;
		std::map<int,
			std::vector<VServer*> >	_fd_to_vserver;
		std::map<int, int>          _client_to_server;
		std::map<int, Request*>     _fd_to_request;
		std::map<int, Response*>    _fd_to_response;

		int                         _client_fd;
		struct sockaddr_in          _client_addr;
		socklen_t                   _client_addr_len = sizeof(_client_addr);

		// 
		void initPollFds();
		void connectionHandler(int fd_index);
		void requestHandler(int fd_index);
		void responseHandler(int fd_index);
		bool isServerFd(int fd);
		VServer& getClientVServer(int client_fd);
	};

} // namespace ws


