#include "FdOperatorManager.h"

std::once_flag FdOperatorManager::once_;
FdOperatorManager* FdOperatorManager::fd_operator_manager_ptr_ = NULL;

void FdOperatorManager::insertWriteFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr)
{
	std::lock_guard<std::mutex> lock(mtx_);
	writeFdopMap[file_path].insert(fdop_ptr);
}
void FdOperatorManager::insertReadFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr)
{
	std::lock_guard<std::mutex> lock(mtx_);
	readFdopMap[file_path].insert(fdop_ptr);
}

void FdOperatorManager::eraseWriteFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr)
{
	std::lock_guard<std::mutex> lock(mtx_);
	if(writeFdopMap.find(file_path) != writeFdopMap.end())
	{ 
		writeFdopMap[file_path].erase(fdop_ptr);
		if(writeFdopMap[file_path].empty())
		{
			writeFdopMap.erase(file_path);
			if(readFdopMap.find(file_path) != readFdopMap.end())
			{
				for(auto item : readFdopMap[file_path])
				{
					item->enableReading();
				}
			}
		}
	}
}

void FdOperatorManager::eraseReadFdop(const std::string& file_path, std::shared_ptr<FdOperator> fdop_ptr)
{
	std::lock_guard<std::mutex> lock(mtx_);
	if(readFdopMap.find(file_path) != readFdopMap.end())
	{ 
		readFdopMap[file_path].erase(fdop_ptr);
		if(readFdopMap[file_path].size() == 0)
		{
			readFdopMap.erase(file_path);
		}
	}
}
