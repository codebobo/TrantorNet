#ifndef TRANTOR_EPOLL_H_
#define TRANTOR_EPOLL_H_

#include <sys/epoll.h>
#include <vector>
#include <map>

using namespace std;

class TrantorEPoll
{
public:
	explicit TrantorEPoll(uint32_t max_fd_num);
	void registerFd(uint32_t fd, uint32_t events);
	void removeFd(uint32_t fd);
	void startEPoll(map<uint32_t, uint32_t>* event_map);
private:
	uint32_t epfd_;
	uint32_t max_fd_num_;
	vector<epoll_event> events_vec;
};

#endif

