#ifndef FD_LOG_H_
#define FD_LOG_H_

#include <string>
#include <memory>
#include <thread>
#include "FdLoop.h"
#include "FdOperator.h"
#include "StringTraits.h"
#include "TrantorTimestamp.h"

namespace trantor
{
#define log_debug log(DEBUG, __FILE__, __LINE__)
#define log_info log(INFO, __FILE__, __LINE__)
#define log_warn log(WARN, __FILE__, __LINE__)
#define log_error log(ERROR, __FILE__, __LINE__)
#define log_fatal log(FATAL, __FILE__, __LINE__)

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
		std::call_once(FdLog::once_, &FdLog::init);
		return *FdLog::fd_log_ptr_;
	}
	bool setAsyncLogFilePath(const std::string& file_path);
	void log(const std::string& log_content);
	void setLogLevel(const LOG_LEVEL level)
	{
		current_log_level_ = level;
	}
	void generateLogSuffix(const std::string& file_name, const int line_num);
	void generateLogPrefix(const LOG_LEVEL level);

	template<class T>
	FdLog& operator<<(T content)
	{
		log(StringTraits(content));
		return *this;
	}

private:
	std::shared_ptr<FdLoop> fd_loop_ptr_;
	std::shared_ptr<FdOperator> fd_operator_ptr_;
	std::mutex mtx_;
	std::string log_suffix_;
	std::string log_prefix_;
	LOG_LEVEL current_log_level_;
	
	static void init()
	{
		if(!FdLog::fd_log_ptr_)
		{
			FdLog::fd_log_ptr_ = new FdLog();
		}
	}
	static FdLog* fd_log_ptr_;
	static std::once_flag once_;
	FdLog();
	FdLog(FdLog&) = delete;
};

void setAsyncLogLevel(LOG_LEVEL level);
bool setAsyncLogPath(const std::string& path);
FdLog& log(const LOG_LEVEL level, const std::string& file_name, const int line_num);

}


#endif
