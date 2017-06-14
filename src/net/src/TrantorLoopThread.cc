#include "TrantorLoopThread.h"


TrantorLoop* TrantorLoopThread::getLoop()
{
	return &loop_;
}

void TrantorLoopThread::start()
{
	loop_thread_ = new std::thread(std::bind(&TrantorLoopThread::threadFunc, this)); 
}

void TrantorLoopThread::threadFunc()
{
	loop_.loop();
}