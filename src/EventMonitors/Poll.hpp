#pragma once

#include "IEventMonitor.hpp"
#include <vector>
#include <poll.h>

#define MAX_FDS 1024

namespace ws
{
	class Poll : public IEventMonitor
	{
	public:

		Poll();
		~Poll();
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
		std::vector<struct pollfd> _pollfds;
		int _nfds;
	};
} // namespace ws
