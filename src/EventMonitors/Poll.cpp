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
			std::vector<struct Listen>::const_iterator
			it = servers[i]->getListens().begin();	
			for (; it != servers[i]->getListens().end(); it++)
			{
				if (it->fd == -1)
					continue;
				struct pollfd pfd = {it->fd, POLLIN, 0};
				_pollfds.push_back(pfd);
				_nfds++;
				_server_fds.insert(it->fd);
			}
		}
	}

	int Poll::monitor()
	{
		return ::poll(&_pollfds.front(), _pollfds.size(), -1);
	}

	void Poll::addEvent(int fd)
	{
		struct pollfd pfd = {fd, POLLIN, 0};
		_pollfds.push_back(pfd);
		_nfds++;
	}

	void Poll::setWriteEvent(int fd_index)
	{
		_pollfds.at(fd_index).events = POLLOUT;
		_pollfds.at(fd_index).revents = 0;
	}

	void Poll::removeEvent(int fd_index)
	{
		_pollfds.erase(_pollfds.begin() + fd_index);
		_nfds--;
	}

	bool Poll::isRead(int _event_index)
	{
		return _pollfds.at(_event_index).revents & POLLIN;
	}

	bool Poll::isWrite(int _event_index)
	{
		return _pollfds.at(_event_index).revents & POLLOUT;
	}

	bool Poll::isError(int _event_index)
	{
		return _pollfds.at(_event_index).revents & POLLHUP
			|| _pollfds.at(_event_index).revents & POLLERR
			|| _pollfds.at(_event_index).revents & POLLNVAL;
		// return _pollfds.at(_event_index).revents & (POLLHUP | POLLERR | POLLNVAL);
		// return _pollfds.at(_event_index).revents & POLLERR;
	}

	size_t Poll::size() const
	{
		return _pollfds.size();
		// return _nfds;
	}

	int Poll::getFd(int index) const
	{
		// if (index < 0 || index >= _nfds)
		// 	return -1;
		return _pollfds.at(index).fd;
	}
} // namespace ws