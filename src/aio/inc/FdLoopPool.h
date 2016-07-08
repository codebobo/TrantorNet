#ifndef FD_LOOP_POOL_H_
#define FD_LOOP_POOL_H_

#include <vector>
#include "FdLoop.h"

namespace trantor
{
	class FdLoopPool
	{
	public:
		explicit FdLoopPool(const int loop_num, const uint64_t timeout_ms, const uint64_t max_fd_num);
		std::shared_ptr<FdLoop> getLoop() const;
	private:
		std::vector<std::shared_ptr<FdLoop> > loop_vec_;
		int current_index_;
		int loop_num_;
	};
}

#endif
