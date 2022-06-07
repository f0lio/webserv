#include "webserv.hpp"

namespace ws
{
	void addFd(int fd)
	{
		_pollfds[_nfds].fd = fd;
		_pollfds[_nfds].events = POLLIN;
		_pollfds[_nfds].revents = 0;
	}

	void setToWrite(int fd_index)
	{
		_pollfds[fd_index].events = POLLOUT;
		_pollfds[fd_index].revents = 0;
	}

	void removeFd(int fd_index)
	{
		_pollfds[fd_index].events = 0;
		_pollfds[fd_index].revents = 0;
		_pollfds[fd_index].fd = -1;
	}

	void initMonitor()
	{
		std::vector<VServer *> const &servers = _config.getVServers();

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
			}
		}
	}

	bool isRead(int _event_index)
	{
		return _pollfds[i].revents & POLLIN;
	}

	bool isWrite(int _event_index)
	{
		return _pollfds[i].revents & POLLOUT;
	}

} // namespace ws
