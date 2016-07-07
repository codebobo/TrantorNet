#include "TrantorTimer.h"
#include <sys/timerfd.h>
#include <vector>
#include "muduo/base/Logging.h"
#include "TrantorLoop.h"

namespace trantor
{
	TrantorTimer::TrantorTimer(TrantorLoop *loop)
	:next_trigger_time_(0),
	loop_(loop)
	{
		assert(loop_ != NULL);
		timerfd_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC); 

		if(timerfd_ > 0)
		{
			timer_pipe_sp_ = make_shared<TrantorPipe>(timerfd_);
			if(timer_pipe_sp_)
			{
				timer_pipe_sp_->enableRead();
				timer_pipe_sp_->setReadCb(bind(&TrantorTimer::handleRead, this));
			}
			else
			{
				LOG_FATAL<<"make share ptr failed";
				abort();
			}
		}
		else
		{
			LOG_FATAL<<"timerfd create failed";
			abort();
		}
	}

	void TrantorTimer::handleRead()
	{
		//LOG_DEBUG<<"timer trigger";
		uint64_t one = 1;
		uint64_t n = read(timerfd_, &one, sizeof one);
		if (n != sizeof one)
		{
			LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
		}

		runTimerCb();
		next_trigger_time_ = TrantorTimestamp(0);
		if(!time_cb_map_.empty())
		{
			registerTimer(time_cb_map_.begin()->first);
		}
	}

	void TrantorTimer::runTimerCb()
	{
		vector<pair<TrantorTimestamp, TimerCallback> > cb_to_do_;
		auto end = time_cb_map_.lower_bound(TrantorTimestamp::now());
		copy(time_cb_map_.begin(), end, back_inserter(cb_to_do_));
		time_cb_map_.erase(time_cb_map_.begin(), end);
		for(auto iter = cb_to_do_.begin(); iter != cb_to_do_.end(); ++iter)
		{
			//LOG_DEBUG<<iter->first.transToString();
			(iter->second)();
		}
	}

	void TrantorTimer::runAt(const TrantorTimestamp& time, const TimerCallback& cb)
	{
		if(loop_)
		{
			loop_->runInLoop(bind(&TrantorTimer::registerTimerCbInLoop, this, time, cb));
		}
		else
		{
			LOG_ERROR<<"loop null";
		}
	}
	void TrantorTimer::runAfter(const double interval, const TimerCallback& cb)
	{
		TrantorTimestamp trigger_time = TrantorTimestamp::now() + interval * 1000000;
		runAt(trigger_time, cb);
	}
	void TrantorTimer::runEvery(const double interval, const TimerCallback& cb)
	{
		//LOG_DEBUG<<"run every";
		auto f = [=]()
		{
			//LOG_DEBUG<<"run cb "<<interval;
			cb();
			runEvery(interval, cb);
		};
		runAfter(interval, f);
	}

	void TrantorTimer::registerTimerCbInLoop(const TrantorTimestamp& trigger_time, const TimerCallback& cb)
	{
		//LOG_DEBUG<<"register new timer cb in loop "<<trigger_time.transToString();
		time_cb_map_.insert(make_pair(trigger_time, cb));
		if(trigger_time < next_trigger_time_ || next_trigger_time_.getTimeMicroSeconds() == 0)
		{
			//LOG_DEBUG<<"register new timer in loop";
			registerTimer(trigger_time);
		}
	}

	uint64_t TrantorTimer::getIntervalFromNow(const TrantorTimestamp& trigger_time)
	{
		int64_t micro_seconds = trigger_time - TrantorTimestamp::now();
		micro_seconds = micro_seconds < 100 ? 100 : micro_seconds;
		return static_cast<uint64_t>(micro_seconds);
	}

	void TrantorTimer::registerTimer(const TrantorTimestamp& trigger_time)
	{
		uint64_t micro_seconds =  getIntervalFromNow(trigger_time);
		//LOG_DEBUG<<"register new timer "<<micro_seconds;

		struct itimerspec trigger_interval;
		bzero(&trigger_interval,sizeof(trigger_interval));
		trigger_interval.it_value.tv_sec = micro_seconds / 1000000;
		trigger_interval.it_value.tv_nsec = (micro_seconds - trigger_interval.it_value.tv_sec * 1000000) * 1000;
		timerfd_settime(timerfd_, 0, &trigger_interval, NULL);
		next_trigger_time_ = trigger_time;
	}
}
