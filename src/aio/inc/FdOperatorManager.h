#ifndef FD_OPERATOR_MANAGER_H_
#define FD_OPERATOR_MANAGER_H_

#include <mutex>
#include "TrantorSingleton.h"
#include "FdOperator.h"

class FdOperatorManager
{
public:
	friend class TrantorSingleton<FdOperatorManager>;

	static FdOperatorManager& instance()
	{
		return TrantorSingleton<FdOperatorManager>::Instance();
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

	explicit FdOperatorManager() = default;
};

#endif
