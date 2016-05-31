#ifndef TRANTOR_LOOP_H_
#define TRANTOR_LOOP_H_

#include <mutex>
#include <thread>
#include <atomic>
#include "TrantorEPoll.h"
#include "TrantorPipe.h"
#include "TrantorEvent.h"
#include "TrantorTimer.h"

namespace trantor
{
	class TrantorLoop
	{
	public:
		explicit TrantorLoop(uint64_t max_fd_num);
		~TrantorLoop();
		bool isInLoopThread() const 
		{ 
			return loop_thread_id_ == std::this_thread::get_id(); 
		}
		void start()
		{
			loop_thread_ = std::thread(&TrantorLoop::loopThread, this);
		}

		void runInLoop(std::function<void()> func);
		void registerPipe(shared_ptr<TrantorPipe> pipe_ptr);
		void removePipe(shared_ptr<TrantorPipe> pipe_ptr);

		void runAt(const TrantorTimestamp& time, const TimerCallback& cb)
		{
			timer_.runAt(time, cb);
		}
		void runAfter(const double interval, const TimerCallback& cb)
		{
			timer_.runAfter(interval, cb);
		}
		void runEvery(const double interval, const TimerCallback& cb)
		{
			timer_.runEvery(interval, cb);
		}

	private:
		TrantorEPoll epoll_;
		TrantorEvent event_;
		TrantorTimer timer_;

		std::vector<std::function<void ()> > loop_funcs_list_;
		std::mutex mtx_;
		std::thread loop_thread_;
		bool need_wakeup_;
		std::thread::id loop_thread_id_;
		std::atomic<bool> loop_alive_;
		map<uint32_t, shared_ptr<TrantorPipe> > pipe_map_;

		void runLoopFuncs();
		void queueInLoop(std::function<void()> func);
		void wakeup();
		void loopThread();
		void registerPipeInLoop(shared_ptr<TrantorPipe> pipe_ptr);
		void removePipeInLoop(shared_ptr<TrantorPipe> pipe_ptr);
		void handleEvents(map<uint32_t, uint32_t>* event_map);
	};
}


#endif

