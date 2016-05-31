#include "TrantorPipe.h"
#include "muduo/base/Logging.h"

namespace trantor
{
	TrantorPipe::TrantorPipe(const uint32_t fd)
	:fd_(fd),
	register_event_(0)
	{
	}

	void TrantorPipe::setReadCb(const function <void()>& read_cb)
	{
		read_cb_ = read_cb;
	}

	void TrantorPipe::setWriteCb(const function <void()>& write_cb)
	{
		write_cb_ = write_cb;
	}

	void TrantorPipe::handleRead()
	{
		if(read_cb_)
		{
			read_cb_();
		}
		else
		{
			LOG_WARN<<"undefine read cb";
		}
	}

	void TrantorPipe::handleWrite()
	{
		if(write_cb_)
		{
			write_cb_();
		}
		else
		{
			LOG_WARN<<"undefine write cb";
		}
	}

	void TrantorPipe::handleEvent(uint32_t revent)
	{
		if(revent & TReadEvent)
		{
			handleRead();
		}
		if(revent & TWriteEvent)
		{
			handleWrite();
		}
	}

	void TrantorPipe::enableRead()
	{
		register_event_ |= TReadEvent;
	}

	void TrantorPipe::enableWrite()
	{
		register_event_ |= TWriteEvent;
	}
}
