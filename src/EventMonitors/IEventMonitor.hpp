#pragma once

#include "../Networking/VServer.hpp"

// inteface for event monitors
namespace ws
{
	class IEventMonitor
	{
	public:
		virtual int monitor()= 0;
		virtual void addEvent(int fd) = 0;
		virtual void removeEvent(int fd_index) = 0;
		virtual void setWriteEvent(int fd_index) = 0;
		virtual bool isRead(int _event_index) = 0;
		virtual bool isWrite(int _event_index) = 0;
		virtual bool isError(int _event_index) = 0;
		virtual int getFd(int index) const = 0;
		virtual size_t size() const = 0;
	};
}
