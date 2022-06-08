#include "KQueue.hpp"

namespace ws
{
	KQueue::KQueue() {};
	KQueue::~KQueue() {}
	
	KQueue::setup(
		std::vector<VServer*> const& servers,
		std::set<int>& _server_fds)
	{
		_nfds = 0;
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::vector<struct Listen>::const_iterator it = servers[i]->getListens().begin();
			for (; it != servers[i]->getListens().end(); it++)
			{
				if (it->fd == -1)
					continue;
				_kevents[_nfds].ident = it->fd;
				_kevents[_nfds].filter = EVFILT_READ;
				_kevents[_nfds].flags = EV_ADD | EV_ENABLE;
				_kevents[_nfds].fflags = 0;
				_kevents[_nfds].data = 0;
				_kevents[_nfds].udata = NULL;
				_nfds++;
				_server_fds.insert(it->fd);
			}
		}
	}

	int KQueue::monitor()
	{
		return ::kevent(_kq, _kevents, _nfds, _kevents, _nfds, NULL);
	}

	void KQueue::addEvent(int fd)
	{
		_kevents[_nfds].ident = fd;
		_kevents[_nfds].filter = EVFILT_READ;
		_kevents[_nfds].flags = EV_ADD | EV_ENABLE;
		_kevents[_nfds].fflags = 0;
		_kevents[_nfds].data = 0;
		_kevents[_nfds].udata = NULL;
		_nfds++;
	}

	void KQueue::setWriteEvent(int fd_index)
	{
		_kevents[fd_index].filter = EVFILT_WRITE;
		_kevents[fd_index].flags = EV_ADD | EV_ENABLE;
		_kevents[fd_index].fflags = 0;
		_kevents[fd_index].data = 0;
		_kevents[fd_index].udata = NULL;
	}

	void KQueue::removeEvent(int fd_index)
	{
		_kevents[fd_index].flags = EV_DELETE;
		_kevents[fd_index].fflags = 0;
		_kevents[fd_index].data = 0;
		_kevents[fd_index].udata = NULL;
	}

	bool KQueue::isRead(int _event_index)
	{
		return _kevents[_event_index].flags & EV_ERROR;
	}

	bool KQueue::isWrite(int _event_index)
	{
		return _kevents[_event_index].flags & EV_ERROR;
	}

	bool KQueue::isError(int _event_index)
	{
		return _kevents[_event_index].flags & EV_ERROR;
	}
	size_t KQueue::size() const
	{
		return _nfds;
	}

	int Poll::getFd(int index) const
	{
		// if (index < 0 || index >= _nfds)
		// 	return -1;
		return _kevents[index].ident;
	}
} // namespace ws
