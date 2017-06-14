#ifndef TRANTOR_TIMER_LOOP_H_
#define TRANTOR_TIMER_LOOP_H_

#include "TrantorLoop.h"

namespace trantor
{
	class TrantorTimerLoop
	{
	public:
		static TrantorTimerLoop& Instance()
		{
			std::call_once(once_, &TrantorTimerLoop::init);
			return *timer_loop_ptr_;
		}
		void runAt(const TrantorTimestamp& time, const TimerCallback& cb)
		{
			loop_.runAt(time, cb);
		}
		void runAfter(const double interval, const TimerCallback& cb)
		{
			loop_.runAfter(interval, cb);
		}
		void runEvery(const double interval, const TimerCallback& cb)
		{
			loop_.runEvery(interval, cb);
		}
	private:

		static std::once_flag once_;
		static TrantorTimerLoop* timer_loop_ptr_;
		static void init()
		{
			if(!timer_loop_ptr_)
			{
				timer_loop_ptr_ = new TrantorTimerLoop;
			}
		}

		explicit TrantorTimerLoop()
		:loop_(2)
		{
			loop_.loop();
		}
		TrantorLoop loop_;
	};
}

#endif

