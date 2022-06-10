#pragma once

#include "IEventMonitor.hpp"
#include <sys/epoll.h>

#define MAX_FDS 1024

namespace ws
{
	class EPoll : public IEventMonitor
	{
	public:

		EPoll();
		~EPoll();
		virtual void setup(
			std::vector<VServer*> const& servers,
			std::set<int>& _server_fds);
		virtual int monitor();
		virtual void addEvent(int fd);
		virtual void removeEvent(int fd_index);
		virtual void setWriteEvent(int fd_index);
		virtual bool isRead(int _event_index);
		virtual bool isWrite(int _event_index);
		virtual bool isError(int _event_index);
		virtual int getFd(int index) const;
		virtual size_t size() const;
	private:
		int _epoll_fd;
		struct epoll_event _events[MAX_FDS];
		int _nfds;
	};
} // namespace ws
