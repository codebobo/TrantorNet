#include "TrantorLoop.h"
#include "FdLog.h"

namespace trantor
{
	TrantorLoop::TrantorLoop(uint64_t max_fd_num)
	:epoll_(max_fd_num),
	need_wakeup_(false),
	loop_alive_(true),
	event_(this),
	timer_(this)
	{
	}

	TrantorLoop::~TrantorLoop()
	{
		loop_alive_ = false;
		//loop_thread_.join();
	}

	void TrantorLoop::loop()
	{
		loop_thread_id_ = std::this_thread::get_id();
		registerPipeInLoop(event_.getEventPipe());
		registerPipeInLoop(timer_.getTimerPipe());
		while(loop_alive_) 
		{
			map<uint32_t, uint32_t> event_map;
			epoll_.startEPoll(&event_map);
			handleEvents(&event_map);
			runLoopFuncs();
		}
	}

	void TrantorLoop::handleEvents(map<uint32_t, uint32_t>* event_map)
	{
		if(event_map)
		{
			for(auto iter = event_map->cbegin(); iter != event_map->cend(); ++iter)
			{
				if(pipe_map_.find(iter->first) != pipe_map_.end())
				{
					//LOG_DEBUG<<"event fd: "<<iter->first;
					pipe_map_[iter->first]->handleEvent(iter->second);
				}
			}
		}
	}

	void TrantorLoop::runLoopFuncs()
	{
		need_wakeup_ = true;
		std::vector<std::function<void ()> > funcs;
		{
			lock_guard<mutex> lock(mtx_);
			funcs.swap(loop_funcs_list_);
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

	void TrantorLoop::runInLoop(std::function<void()> func)
	{
		//LOG_DEBUG<<"run in loop";
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

	void TrantorLoop::queueInLoop(std::function<void()> func)
	{
		//LOG_DEBUG<<"queue in loop";
		{
			lock_guard<mutex> lock(mtx_);
			loop_funcs_list_.push_back(func);
		}
		if(!isInLoopThread() || need_wakeup_)
		{
			//log_debug<<"wake up loop";
			wakeup();
		}
	}

	void TrantorLoop::wakeup()
	{
		event_.trigEvent();
	}

	void TrantorLoop::updatePipe(shared_ptr<TrantorPipe> pipe_ptr)
	{
		//log_debug<<"register pipe";
		std::function<void()> func = std::bind(&TrantorLoop::updatePipeInLoop, this, pipe_ptr);
		runInLoop(func);
	}

	void TrantorLoop::updatePipeInLoop(shared_ptr<TrantorPipe> pipe_ptr)
	{
		//log_debug<<"register pipe in loop "<<pipe_ptr->getFd()<<" "<<pipe_ptr->getEvent();
		if(pipe_ptr)
		{
			if(pipe_map_.find(pipe_ptr->getFd()) != pipe_map_.end())
			{
				epoll_.updateFd(pipe_ptr->getFd(), pipe_ptr->getEvent());
			}
			else
			{
				log_warn<<"pipe has not been registered";
			}
		}
		else
		{
			log_error<<"pointer null";
		}
	}

	void TrantorLoop::registerPipe(shared_ptr<TrantorPipe> pipe_ptr)
	{
		//log_debug<<"register pipe";
		std::function<void()> func = std::bind(&TrantorLoop::registerPipeInLoop, this, pipe_ptr);
		runInLoop(func);
	}

	void TrantorLoop::registerPipeInLoop(shared_ptr<TrantorPipe> pipe_ptr)
	{
		//log_debug<<"register pipe in loop "<<pipe_ptr->getFd()<<" "<<pipe_ptr->getEvent();
		if(pipe_ptr)
		{
			pipe_map_[pipe_ptr->getFd()] = pipe_ptr;
			epoll_.registerFd(pipe_ptr->getFd(), pipe_ptr->getEvent());
		}
		else
		{
			log_error<<"pointer null";
		}
	}

	void TrantorLoop::removePipe(shared_ptr<TrantorPipe> pipe_ptr)
	{
		function<void()> func = bind(&TrantorLoop::removePipeInLoop, this, pipe_ptr);
		runInLoop(func);
	}

	void TrantorLoop::removePipeInLoop(shared_ptr<TrantorPipe> pipe_ptr)
	{
		if(pipe_ptr)
		{
			pipe_map_.erase(pipe_ptr->getFd());
			epoll_.removeFd(pipe_ptr->getFd());
		}
		else
		{
			//LOG_ERROR<<"pointer null";
		}
	}
}





