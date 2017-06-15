#include <sys/eventfd.h>
#include <iostream>
#include "FdPoll.h"

FdPoll::FdPoll() 
{
	wakeup_fd_ = createEventfd();
}

void FdPoll::startPoll(uint64_t timeoutMs)
{
	initEvents();

	int nfds = ::poll((struct pollfd *)(&(*events_.begin())), events_.size(), timeoutMs);
	 if (nfds < 0) 
	 {                                 
		 perror ("poll");
		 return;
	 }
	if (nfds == 0) 
	{
		//LOG_DEBUG<<"no events ";
		return;
	}
	else
	{
		//LOG_DEBUG<<"events num: "<<nfds;
	}

	handleEvents();
}

void FdPoll::handleEvents()
{
	//std::cout<<"fd loop handle events"<<std::endl;
	for(auto iter = events_.begin(); iter != events_.end(); ++iter)
	{
		if(iter->fd == wakeup_fd_ && iter->revents)
		{
			uint64_t one = 1;
			uint64_t n = read(wakeup_fd_, &one, sizeof one);
			if (n != sizeof one)
			{
				//LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
			}
		}
		else
		{
			FdPtr fdptr = fd_map_[iter->fd];
			if(fdptr && iter->revents > 0)
			{
				fdptr->setRevents(iter->revents);
				fdptr->handleEvents();
			}
		}
	}
}

void FdPoll::wakeup()
{
	//std::cout<<"wake up"<<wakeup_fd_<<std::endl;
    uint64_t one = 1;
    uint64_t n = write(wakeup_fd_, &one, sizeof one);
    if (n != sizeof one)
    {
		//std::cout<<"wakeup fd: "<<wakeup_fd_<<std::endl;
		perror("wakeup log: ");
    }
}

void FdPoll::initEvents()
{
	events_.clear();
	struct pollfd newfd;
	newfd.events = POLLIN | POLLPRI;
	newfd.revents = 0;
	newfd.fd = wakeup_fd_;
	events_.push_back(newfd);

	for(auto iter = fd_map_.begin(); iter != fd_map_.end(); ++iter)
	{
		if(iter->second)
		{
			struct pollfd newfd;
			newfd.events = (iter->second)->getEvents();
			newfd.revents = 0;
			newfd.fd = (iter->second)->getFd();
			//std::cout<<"initevents: "<<newfd.fd<<std::endl;
			events_.push_back(newfd);
		}
	}
}

uint64_t FdPoll::createEventfd()
{
	uint64_t evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		//LOG_SYSERR << "Failed in eventfd";
		abort();
	}
	return evtfd;
}

void FdPoll::registerFdInLoop(FdPtr fdptr)
{
	if(fdptr)
	{
		uint64_t fd = fdptr->getFd();
		if(fd >= 0)
		{
			//std::cout<<"register fd: "<<fd<<std::endl;
			fd_map_[fd] = fdptr;
		}
	}
}
void FdPoll::deleteFdInLoop(FdPtr fdptr)
{
	if(fdptr)
	{
		uint64_t fd = fdptr->getFd();
		if(fd >= 0)
		{
			//LOG_DEBUG<<"delete fd: "<<fd;
			fd_map_.erase(fd);
		}
	}
}


