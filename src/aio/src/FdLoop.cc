#include "FdLoop.h"

FdLoop::FdLoop(uint64_t timeoutMs, uint64_t maxFdNum)
:poll_(),
time_out_ms_(timeoutMs),
need_wakeup_(false),
loop_thread_(),
loop_alive_(true)
{

}

FdLoop::~FdLoop()
{
	loop_alive_ = false;
	loop_thread_.join();
}

void FdLoop::threadFunc(uint64_t timeoutMs)
{
	loop_thread_id_ = std::this_thread::get_id();
	while(loop_alive_) 
	{
		poll_.startPoll(timeoutMs);
		runFuncs();
	}
}

void FdLoop::runFuncs()
{
	need_wakeup_ = true;
	std::vector<std::function<void ()> > funcs;
	{
		std::lock_guard<std::mutex> lock(mtx_);
		funcs.swap(funcs_list_);
	}
	for(int i = 0; i < funcs.size(); i++)
	{
		if(funcs[i])
		{
			funcs[i]();
		}
	}
	need_wakeup_ = false;
}

void FdLoop::runInLoop(std::function<void()> func)
{
	if(func)
	{
		if(isInLoopThread())
		{
			func();
		}
		else
		{
			queueInLoop(func);
		}
	}
}

void FdLoop::queueInLoop(std::function<void()> func)
{
	{
		std::lock_guard<std::mutex> lock(mtx_);
		funcs_list_.push_back(func);
	}
	if(!isInLoopThread() || need_wakeup_)
	{
		wakeup();
	}
}

void FdLoop::wakeup()
{
	poll_.wakeup();
}

void FdLoop::registerFd(FdPtr fdptr)
{
	std::function<void()> func = std::bind(&FdPoll::registerFdInLoop, &poll_, fdptr);
	runInLoop(func);
}

void FdLoop::deleteFd(FdPtr fdptr)
{
	std::function<void()> func = std::bind(&FdPoll::deleteFdInLoop, &poll_, fdptr);
	runInLoop(func);
}
