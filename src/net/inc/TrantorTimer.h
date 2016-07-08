#ifndef TRANTOR_TIMER_H_
#define TRANTOR_TIMER_H_

#include <stdint.h>
#include <thread>
#include <functional>
#include <map>
#include <mutex>
#include "TrantorTimestamp.h"
#include "TrantorPipe.h"

using namespace std;
using namespace trantor;

namespace trantor
{
	class TrantorLoop;
	typedef function<void()> TimerCallback;

	class TrantorTimer
	{
	public:
		explicit TrantorTimer(TrantorLoop *loop);
		void runAt(const TrantorTimestamp& time, const TimerCallback& cb);
		void runAfter(const double interval, const TimerCallback& cb);
		void runEvery(const double interval, const TimerCallback& cb);
		shared_ptr<TrantorPipe> getTimerPipe() const{return timer_pipe_sp_;}

	private:
		void handleRead();
		void runTimerCb();
		void registerTimerCbInLoop(const TrantorTimestamp& trigger_time, const TimerCallback& cb);
		void registerTimer(const TrantorTimestamp& trigger_time);
		uint64_t getIntervalFromNow(const TrantorTimestamp& trigger_time);

		int timerfd_;
		TrantorTimestamp next_trigger_time_;
		shared_ptr<TrantorPipe> timer_pipe_sp_;
		TrantorLoop *loop_;
		multimap<TrantorTimestamp, TimerCallback> time_cb_map_;
	};
}

#endif
