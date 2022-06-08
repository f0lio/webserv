#include "Poll.hpp"

namespace ws
{
	Poll::Poll() {};

	Poll::~Poll() {}

	void Poll::setup(
			std::vector<VServer*> const& servers,
			std::set<int>& _server_fds)
	{
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

	int Poll::monitor()
	{
		return ::poll(_pollfds, _nfds, -1);
	}

	void Poll::addEvent(int fd)
	{
		_pollfds[_nfds].fd = fd;
		_pollfds[_nfds].events = POLLIN;
		_pollfds[_nfds].revents = 0;
		_nfds++;
	}

	void Poll::setWriteEvent(int fd_index)
	{
		_pollfds[fd_index].events = POLLOUT;
		_pollfds[fd_index].revents = 0;
	}

	void Poll::removeEvent(int fd_index) //not really removing, just disabling
	{
		_pollfds[fd_index].events = 0;
		_pollfds[fd_index].revents = 0;
		_pollfds[fd_index].fd = -1;
	}

	bool Poll::isRead(int _event_index)
	{
		return _pollfds[_event_index].revents & POLLIN;
	}

	bool Poll::isWrite(int _event_index)
	{
		return _pollfds[_event_index].revents & POLLOUT;
	}

	bool Poll::isError(int _event_index)
	{
		return _pollfds[_event_index].revents & POLLERR;
	}

	size_t Poll::size() const
	{
		return _nfds;
	}

	int Poll::getFd(int index) const
	{
		// if (index < 0 || index >= _nfds)
		// 	return -1;
		return _pollfds[index].fd;
	}


} // namespace ws