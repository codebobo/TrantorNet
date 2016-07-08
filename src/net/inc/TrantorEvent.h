#ifndef TRANTOR_EVENT_H_
#define TRANTOR_EVENT_H_

#include <stdint.h>
#include <memory>
#include "TrantorPipe.h"


namespace trantor
{
	class TrantorLoop;
	class TrantorEvent
	{
	public:
		explicit TrantorEvent(TrantorLoop *loop);
		void trigEvent();
		void handleRead();
		shared_ptr<TrantorPipe> getEventPipe() const{return event_pipe_sp_;}
	private:
		uint32_t createEventfd();
		uint32_t eventfd_;
		shared_ptr<TrantorPipe> event_pipe_sp_;
		TrantorLoop* loop_;
	};
}

#endif
