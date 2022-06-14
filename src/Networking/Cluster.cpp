#include "Cluster.hpp"

namespace ws
{
	Cluster::Cluster(Configuration const &config) : _config(config), _running(false), _setup(false)
	{
	}

	Cluster::~Cluster()
	{
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
		_client_fd = accept(_io.getFd(fd_index), (struct sockaddr *)&_client_addr, &_client_addr_len);

		fcntl(_client_fd, F_SETFL, O_NONBLOCK);

		_client_to_server[_client_fd] = _io.getFd(fd_index);

		if (_client_fd == -1)
			throw std::runtime_error("Cluster::run() : accept() failed");
		else
			_io.addEvent(_client_fd);
	}

	void Cluster::requestHandler(int fd_index)
	{
		if (_fd_to_request.find(_io.getFd(fd_index)) == _fd_to_request.end())
		{
			console.log("Cluster::run() : new request", ": ", _io.getFd(fd_index), "\n");

			Request *request = new Request(
				_io.getFd(fd_index), _fd_to_vserver[_client_to_server[_io.getFd(fd_index)]]);
			console.log("Cluster::run() : new request", ": ", _io.getFd(fd_index), "\n");
			_fd_to_request[_io.getFd(fd_index)] = request;
		}
		// console.log("Request handler");
		_fd_to_request[_io.getFd(fd_index)]->process();
		// console.log("Request processed");
		
		_io.setWriteEvent(fd_index);
	}

	void Cluster::responseHandler(int fd_index)
	{
		// console.log("Response handler");
		if (_fd_to_request[_io.getFd(fd_index)]->isComplete())
		{	
			console.log("Cluster::run() : new response", ": ", _io.getFd(fd_index), "\n");
			if (_fd_to_response.find(_io.getFd(fd_index)) == _fd_to_response.end())
			{
				Response *response = new Response(*_fd_to_request[_io.getFd(fd_index)], this->_config);
				_fd_to_response[_io.getFd(fd_index)] = response;
			}
		}
		else
		{
			// console.log("Response handler : request not complete");
			// close(_io.getFd(fd_index));
			return;
		}
		// console.log("Response handler : request complete");

		// console.log("Response handler : checking if response is sent");
		if (_fd_to_response.find(_io.getFd(fd_index))->second->isSent())
		{
			// console.log("Cluster::responseHandler() : response sent", "\n");

			delete _fd_to_request[_io.getFd(fd_index)];
			_fd_to_request.erase(_io.getFd(fd_index));

			delete _fd_to_response[_io.getFd(fd_index)];
			_fd_to_response.erase(_io.getFd(fd_index));

			close(_io.getFd(fd_index));
			_io.removeEvent(fd_index);
		}
		else
		{
			_fd_to_response[_io.getFd(fd_index)]->process();
			_fd_to_response[_io.getFd(fd_index)]->send();
		}
	}

	void Cluster::setup()
	{
		std::vector<VServer *> const &servers = _config.getVServers();

		std::vector<VServer *>::const_iterator it = servers.begin();
		for (; it != servers.end(); it++)
		{
			(*it)->start(_binded_listens);
			std::set<int>::const_iterator it3 = (*it)->getFds().begin();
			for (; it3 != (*it)->getFds().end(); it3++)
				_fd_to_vserver[*it3].push_back(*it);
		}
		_io.setup(servers, this->_server_fds);	
		_setup = true;
	}

	/// Main loop ///
	void Cluster::run()
	{
		if (_setup == false)
			throw std::runtime_error("Cluster::run() : setup() must be called before run()");

		_running = true;
		while (_running)
		{
			console.log("monitoring...");
			int ret = _io.monitor();
			console.log("monitoring done");
			if (ret == -1)
				throw std::runtime_error("Cluster::run() : monitor() failed");
			else if (ret == 0)
			{
				console.warn("-- CONTINUE --" + __COUNTER__);
				continue;
			}
			for (size_t i = 0; i < _io.size(); i++)
			{
				if (_io.isError(i))
				{
					close(_io.getFd(i));
					_io.removeEvent(i);
				}
				else if (_io.isRead(i))
				{
					if (isServerFd(_io.getFd(i)))
						connectionHandler(i);
					else
						requestHandler(i);
				}
				else if (_io.isWrite(i))
					responseHandler(i);
			}
		} // while
	}
} // namespace ws
