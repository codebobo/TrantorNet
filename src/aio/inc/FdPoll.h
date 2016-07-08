#ifndef FD_EPOLL_H_
#define FD_EPOLL_H_

#include <sys/poll.h>
#include <map>
#include <vector>
#include "FdOperator.h"

typedef std::shared_ptr<FdOperator> FdPtr;

class FdLoop;

class FdPoll
{
public:
	explicit FdPoll();
	void registerFdInLoop(FdPtr fdptr);
	void deleteFdInLoop(FdPtr fdptr);
	void initEvents();
	void startPoll(uint64_t timeoutMs);
	void handleEvents();
	void wakeup();

private:
	std::vector<struct pollfd> events_;
	std::map<uint64_t, FdPtr> fd_map_;
	uint64_t wakeup_fd_;

	uint64_t createEventfd();
};

#endif
