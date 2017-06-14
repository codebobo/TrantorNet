#ifndef TRANTOR_PIPE_H_
#define TRANTOR_PIPE_H_

#include <functional>
#include <sys/poll.h>

#define TNoneEvent  0
#define TReadEvent  POLLIN | POLLPRI
#define TWriteEvent  POLLOUT

using namespace std;

namespace trantor
{
	class TrantorPipe
	{
	public:
		explicit TrantorPipe(const uint32_t fd);
		void handleEvent(uint32_t revent);
		void setReadCb(const function <void()>& read_cb);
		void setWriteCb(const function <void()>& write_cb);
		void enableRead();
		void enableWrite();
		void disableRead();
		void disableWrite();
		uint32_t getFd() const {return fd_;}
		uint32_t getEvent() const {return register_event_;}

	private:
		function <void()> write_cb_;
		function <void()> read_cb_;
		uint32_t register_event_;
		uint32_t fd_;

		void handleWrite();
		void handleRead();
	};
}

#endif
