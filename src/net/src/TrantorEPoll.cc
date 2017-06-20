#include "TrantorEPoll.h"
#include <sys/eventfd.h>

TrantorEPoll::TrantorEPoll(uint32_t max_fd_num)
:max_fd_num_(max_fd_num),
events_vec(100)
{
	epfd_ = epoll_create(max_fd_num_);
	if(epfd_ < 0)
	{
		//LOG_ERROR<<"epfd create error";
		abort();
	}
}

void TrantorEPoll::startEPoll(map<uint32_t, uint32_t>* event_map)
{
	int nfds = epoll_wait(epfd_, &(*events_vec.begin()), events_vec.size(), -1);
	for(int i = 0; i < nfds; i++)
	{
		uint32_t fd = events_vec[i].data.fd;
		uint32_t revent = events_vec[i].events;
		event_map->insert(std::make_pair(fd, revent));
	}
	if(nfds == events_vec.size())
	{
		events_vec.resize(2 * events_vec.size());
	}
}

void TrantorEPoll::registerFd(uint32_t fd, uint32_t events)
{
	epoll_event event_st;
	event_st.data.fd = fd;
	event_st.events = events;
	epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event_st);
}

void TrantorEPoll::updateFd(uint32_t fd, uint32_t events)
{
	epoll_event event_st;
	event_st.data.fd = fd;
	event_st.events = events;
	epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event_st);
}


void TrantorEPoll::removeFd(uint32_t fd)
{
	epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, NULL);
}



