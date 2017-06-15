#ifndef FD_LOOP_H_
#define FD_LOOP_H_

#include <mutex>
#include <thread>
#include <atomic>
#include "FdPoll.h"

class FdLoop
{
public:
	explicit FdLoop(uint64_t timeoutMs, uint64_t maxFdNum);
	~FdLoop();
	void threadFunc(uint64_t timeoutMs);
    bool isInLoopThread() const 
	{ 
		return loop_thread_id_ == std::this_thread::get_id(); 
	}
    void start()
	{
		loop_thread_ = std::thread(&FdLoop::threadFunc, this, time_out_ms_);
	}

	void runInLoop(std::function<void()> func);
	void registerFd(FdPtr fdptr);
	void deleteFd(FdPtr fdptr);
	void wakeup();

private:
	FdPoll poll_;
	std::vector<std::function<void ()> > funcs_list_;
	std::mutex mtx_;
	std::thread loop_thread_;
	bool need_wakeup_;
	std::thread::id loop_thread_id_;
	std::atomic<bool> loop_alive_;
	uint64_t time_out_ms_;

	void runFuncs();
	int createEventfd();
	void queueInLoop(std::function<void()> func);
	
};

#endif
