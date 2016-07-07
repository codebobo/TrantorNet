#include "TrantorEvent.h"
#include <sys/eventfd.h>
#include "muduo/base/Logging.h"
#include "TrantorLoop.h"

namespace trantor
{
	TrantorEvent::TrantorEvent(TrantorLoop *loop)
	:eventfd_(createEventfd()),
	loop_(loop)
	{
		if(eventfd_ > 0)
		{
			event_pipe_sp_ = make_shared<TrantorPipe>(eventfd_);
			if(event_pipe_sp_)
			{
				event_pipe_sp_->enableRead();
				event_pipe_sp_->setReadCb(bind(&TrantorEvent::handleRead, this));
			}
			else
			{
				LOG_FATAL<<"make share ptr failed";
				abort();
			}
		}
		else
		{
			LOG_FATAL<<"eventfd create failed";
		}
	}

	uint32_t TrantorEvent::createEventfd()
	{
		uint64_t evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
		if (evtfd < 0)
		{
			LOG_SYSERR << "Failed in eventfd";
			abort();
		}
		return evtfd;
	}

	void TrantorEvent::trigEvent()
	{
		uint64_t one = 1;
		uint64_t n = write(eventfd_, &one, sizeof one);
		if (n != sizeof one)
		{
			LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
		}
	}

	void TrantorEvent::handleRead()
	{
		uint64_t one = 1;
		uint64_t n = read(eventfd_, &one, sizeof one);
		if (n != sizeof one)
		{
			LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
		}
	}
}


