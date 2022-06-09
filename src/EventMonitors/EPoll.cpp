#include "EPoll.hpp"

namespace ws
{
	EPoll::EPoll() {};

	EPoll::~EPoll() {}

	void EPoll::setup(
			std::vector<VServer*> const& servers,
			std::set<int>& _server_fds)
	{
		// _epoll_fd = epoll_create(MAX_FDS);

	}

	int EPoll::monitor()
	{
		return -1;
	}

	void EPoll::addEvent(int fd)
	{
	}

	void EPoll::setWriteEvent(int fd_index)
	{
	}

	void EPoll::removeEvent(int fd_index) //not really removing, just disabling
	{
	}

	bool EPoll::isRead(int _event_index)
	{
		return false;
	}

	bool EPoll::isWrite(int _event_index)
	{
		return false;
	}

	bool EPoll::isError(int _event_index)
	{
		return false;
	}

	size_t EPoll::size() const
	{
		return 0;
	}

	int EPoll::getFd(int index) const
	{
		// if (index < 0 || index >= _nfds)
		// 	return -1;
		return -1;
	}


} // namespace ws