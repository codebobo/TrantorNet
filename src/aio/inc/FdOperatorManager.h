#ifndef FD_OPERATOR_MANAGER_H_
#define FD_OPERATOR_MANAGER_H_

#include <mutex>
#include "FdOperator.h"

class FdOperatorManager
{
public:

	static FdOperatorManager& instance()
	{
		std::call_once(once_, &FdOperatorManager::init);
		return *fd_operator_manager_ptr_;
	}
	void insertWriteFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr);

	void insertReadFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr);

	void eraseWriteFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr);

	void eraseReadFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr);

	bool waitToWrite(const std::string& file_path)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		return writeFdopMap.find(file_path) != writeFdopMap.end();
	}

	void trigReadEvent(const std::string& file_path)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		if(readFdopMap.find(file_path) != readFdopMap.end())
		{
			for(auto item : readFdopMap[file_path])
			{
				item->enableReading();
			}
		}
	}


private:
	std::mutex mtx_;
	FDMAP readFdopMap;
	FDMAP writeFdopMap;
	static std::once_flag once_;
	static FdOperatorManager* fd_operator_manager_ptr_;
	static void init()
	{
		if(!fd_operator_manager_ptr_)
		{
			fd_operator_manager_ptr_ = new FdOperatorManager;
		}
	}

	explicit FdOperatorManager() = default;
};

#endif
