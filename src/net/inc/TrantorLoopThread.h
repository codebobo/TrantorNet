#ifndef TRANTORLOOPTHREAD_H_
#define TRANTORLOOPTHREAD_H_

#include <thread>
#include "TrantorLoop.h"

class TrantorLoopThread
{
	public:
		void start();
		void threadFunc();
		TrantorLoop* getLoop();
		
	private:
		TrantorLoop loop_;
		std::thread* loop_thread_;
};

#endif