#ifndef FD_LOG_H_
#define FD_LOG_H_

#include <string>
#include <memory>
#include <thread>
#include "FdLoop.h"
#include "FdOperator.h"
#include "StringTraits.h"

enum LOG_LEVEL
{
	FATAL = 0,
	ERROR = 1,
	WARN = 2,
	INFO = 3,
	DEBUG = 4,
};

class FdLog
{
public:
	static FdLog& instance()
	{
		std::call_once(once_, &FdLog::init);
		return *fd_log_ptr_;
	}
	bool setAsyncLogFilePath(const std::string& file_path);
	void log(const LOG_LEVEL level, const std::string& log_content);
	void setLogLevel(const LOG_LEVEL level)
	{
		current_log_level_ = level;
	}
private:
	std::shared_ptr<FdLoop> fd_loop_ptr_;
	std::shared_ptr<FdOperator> fd_operator_ptr_;
	LOG_LEVEL current_log_level_;
	static FdLog* fd_log_ptr_;
	static std::once_flag once_;
	static void init()
	{
		if(!fd_log_ptr_)
		{
			fd_log_ptr_ = new FdLog();
		}
	}
	FdLog();
};

void setLogLevel(LOG_LEVEL level);
bool setAsyncLogPath(const std::string& path);


template<class T>
void log(LOG_LEVEL level, T content)
{
	FdLog::instance().log(level, StringTraits<T>(content));
}

#endif
