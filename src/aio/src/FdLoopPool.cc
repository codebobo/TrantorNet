#include "FdLoopPool.h"

namespace trantor
{
	FdLoopPool::FdLoopPool(const int loop_num, const uint64_t timeout_ms, const uint64_t max_fd_num)
	:loop_num_(loop_num),
	current_index_(0),
	loop_vec_()
	{
		for (int i = 0; i < loop_num; i++)
		{
			std::shared_ptr<FdLoop> loop_ptr(new FdLoop(timeout_ms, max_fd_num));
			if(loop_ptr)
			{
				loop_ptr->start();
				loop_vec_.push_back(loop_ptr);
			}
			else
			{
				//LOG_ERROR<<"fd loop ptr null";
			}
		}
	}
	std::shared_ptr<FdLoop> FdLoopPool::getLoop() const
	{
		if(current_index_ < loop_num_)
		{
			return loop_vec_[current_index_];
		}
		else
		{
			//LOG_ERROR<<"index exceed loop num";
			return NULL;
		}
	}
}
