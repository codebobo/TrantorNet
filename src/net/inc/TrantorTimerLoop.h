#ifndef TRANTOR_TIMER_LOOP_H_
#define TRANTOR_TIMER_LOOP_H_

#include "TrantorLoop.h"
#include "TrantorSingleton.h"

namespace trantor
{
	class TrantorTimerLoop
	{
		friend class TrantorSingleton<TrantorTimerLoop>;
	public:
		static TrantorTimerLoop& Instance()
		{
			return TrantorSingleton<TrantorTimerLoop>::Instance();
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
		explicit TrantorTimerLoop()
		:loop_(2)
		{
			loop_.start();
		}
		TrantorLoop loop_;
	};
}

#endif

