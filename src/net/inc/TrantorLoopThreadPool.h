#ifndef TRANTORLOOPTHREADPOOL_H_
#define TRANTORLOOPTHREADPOOL_H_

#include <thread>
#include "TrantorLoopThread.h"

class TrantorLoopThreadPool
{
	public:
		TrantorLoopThreadPool(const int thread_num);
		TrantorLoop* getNextLoop();
		
		
	private:
		std::vector<TrantorLoopThread*> loop_pool_;
		int current_index_;
};

#endif

