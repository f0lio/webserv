#include "KQueue.hpp"

namespace ws
{
	KQueue::KQueue() {};
	KQueue::~KQueue() {}
	
	void KQueue::setup(
		std::vector<VServer*> const& servers,
		std::set<int>& _server_fds)
	{
		_nfds = 0;
		
		// this->_kq = kqueue();
		// if (_kq == -1) {};
		
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::vector<struct Listen>::const_iterator it = servers[i]->getListens().begin();
			for (; it != servers[i]->getListens().end(); it++)
			{
				if (it->fd == -1)
					continue;

				// Later..
				
				_nfds++;
				_server_fds.insert(it->fd);
			}
		}
	}

	int KQueue::monitor()
	{
		return ::kevent(_kq, &_kevents.front() , _kevents.size(), NULL, 0, NULL);
	}

	void KQueue::addEvent(int fd)
	{
		struct kevent ev;
		EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
		_kevents.push_back(ev);
		_nfds++;
		_nfds++;
	}

	void KQueue::setWriteEvent(int fd_index)
	{
		_kevents.at(fd_index).flags = EV_ADD | EV_ENABLE | EV_CLEAR;
		_kevents.at(fd_index).flags = EV_ADD | EV_ENABLE | EV_CLEAR;
	}

	void KQueue::removeEvent(int fd_index)
	{
		_kevents.erase(_kevents.begin() + fd_index);
	}

	bool KQueue::isRead(int _event_index)
	{
		return _kevents[_event_index].filter & EVFILT_READ;
	}

	bool KQueue::isWrite(int _event_index)
	{
		return _kevents[_event_index].filter & EVFILT_WRITE;
	}

	bool KQueue::isError(int _event_index)
	{
		return _kevents[_event_index].filter & EV_ERROR;
	}
	
	bool KQueue::isEOF(int _event_index)
	{
		return _kevents[_event_index].flags & EV_EOF;
	}

	size_t KQueue::size() const
	{
		return _nfds;
	}

	int KQueue::getFd(int index) const
	{
		// if (index < 0 || index >= _nfds)
		// 	return -1;
		return _kevents[index].ident;
	}
} // namespace ws
