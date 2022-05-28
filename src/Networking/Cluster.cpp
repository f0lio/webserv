#include "Cluster.hpp"

namespace ws
{
	Cluster::Cluster(Configuration const& config) : _config(config), _running(false), _setup(false)
	{
	}

	Cluster::~Cluster()
	{
	}

	void Cluster::setup()
	{
		std::vector<VServer*> const& servers = _config.getVServers();

		std::vector<VServer*>::const_iterator it = servers.begin();
		for (; it != servers.end(); it++)
		{
			(*it)->start(_binded_listens);
			// _fd_to_vserver[(*it)->getFd()] = *it;
			_server_fds.insert((*it)->getFd());
		}
		_setup = true;
	}

	/// helper functions ///
	inline bool Cluster::isServerFd(int fd)
	{
		return _server_fds.find(fd) != _server_fds.end();
	}

	VServer &Cluster::getClientVServer(int client_fd)
	{
		
	}

	/// Event handlers ///
	void Cluster::connectionHandler(int fd_index)
	{
		_client_fd = accept(_pollfds[fd_index].fd, (struct sockaddr*)&_client_addr, &_client_addr_len);
		fcntl(_client_fd, F_SETFL, O_NONBLOCK);
		if (_client_fd == -1)
			throw std::runtime_error("Cluster::run() : accept() failed");
		else
		{
			_pollfds[_nfds].fd = _client_fd;
			_pollfds[_nfds].events = POLLIN;
			_pollfds[_nfds].revents = 0;
			_nfds++;
		}
	}

	void Cluster::requestHandler(int fd_index)
	{
		if (_fd_to_request.find(_pollfds[fd_index].fd) == _fd_to_request.end()) //4
		{
			console.log("Cluster::run() : new request", ": ", _pollfds[fd_index].fd, "\n");
			Request *request = new Request(_pollfds[fd_index].fd, _client_addr);
			_fd_to_request[_pollfds[fd_index].fd] = request;
		}
		console.log("Request handler");
		_fd_to_request[_pollfds[fd_index].fd]->process();
		console.log("Request processed");

		_pollfds[fd_index].events = POLLOUT;
		_pollfds[fd_index].revents = 0;
	}

	void Cluster::responseHandler(int fd_index)
	{
		console.log("Response handler");
		console.log("Checking request - fd: ", _pollfds[fd_index].fd, "\n");

		if (_fd_to_request[_pollfds[fd_index].fd]->isComplete())
		{
			if (_fd_to_response.find(_pollfds[fd_index].fd) == _fd_to_response.end())
			{
				console.log("Cluster::run() : new response");
				Response *response = new Response (*_fd_to_request[_pollfds[fd_index].fd], this->_config);
				_fd_to_response[_pollfds[fd_index].fd] = response;
			}
		}
		else
		{
		    console.log("Response handler : request not complete");
			return ;
		}
		console.log("Response handler : request complete");
		
				console.log("Response handler : checking if response is sent");
		if (_fd_to_response.find(_pollfds[fd_index].fd)->second->isSent())
		{
			console.log("Cluster::responseHandler() : response sent", "\n");

			delete _fd_to_request[_pollfds[fd_index].fd];
			_fd_to_request.erase(_pollfds[fd_index].fd);
			

			delete _fd_to_response[_pollfds[fd_index].fd];
			_fd_to_response.erase(_pollfds[fd_index].fd);
			
			close(_pollfds[fd_index].fd);
			_pollfds[fd_index].events = 0;
			_pollfds[fd_index].revents = 0;
			_pollfds[fd_index].fd = -1;
			_nfds--;
		}
		else
		{
			console.log("Response handler : sending response");
			console.log("Cluster::responseHandler() : processing response", "\n");
			_fd_to_response[_pollfds[fd_index].fd]->process();
			console.log("Cluster::responseHandler() : sending response", "\n");
			_fd_to_response[_pollfds[fd_index].fd]->send();
		}
	}

	void Cluster::initPollFds()
	{
		std::vector<VServer*> const& servers = _config.getVServers();

		_nfds = 0;
		memset(&_pollfds, 0, sizeof(_pollfds));
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::vector<struct Listen>::const_iterator it = servers[i]->getListens().begin();
			for (; it != servers[i]->getListens().end(); it++)
			{
				if (it->fd == -1)
					continue;
				_pollfds[_nfds].fd = it->fd;
				_pollfds[_nfds].events = POLLIN;
				_pollfds[_nfds].revents = 0;
				_nfds++;
				_server_fds.insert(it->fd);
				console.log("fd: ", it->fd, " ", it->host, ":", it->port, "\n");
			}
		}
	}

	/// Main loop ///
	void Cluster::run()
	{
		if (_setup == false)
			throw std::runtime_error("Cluster::run() : setup() must be called before run()");

		initPollFds();
		console.log("Cluster started");
		_running = true;
		while (_running)
		{

			console.log("polling...");
			sleep(1);
			int ret = poll(_pollfds, _nfds, -1);
			if (ret == -1)
				throw std::runtime_error("Cluster::run() : poll() failed");
			else if (ret == 0)
				continue;
			for (size_t i = 0; i < _nfds; i++)
			{
				console.log("i: ", i, " fd: ", _pollfds[i].fd, " ", _pollfds[i].revents, "\n");
				if (_pollfds[i].revents & POLLIN)
				{
					if (isServerFd(_pollfds[i].fd))
						connectionHandler(i);
					else
						requestHandler(i);
				}
				else if (_pollfds[i].revents & POLLOUT)
					responseHandler(i);
			}
		} // while
	}
} // namespace ws
