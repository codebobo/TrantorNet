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
#define MAX_SIZE_PER_LOG_FILE 1024 * 1024

#define log_debug FdLog(DEBUG, __FILE__, __LINE__)
#define log_info FdLog(INFO, __FILE__, __LINE__)
#define log_warn FdLog(WARN, __FILE__, __LINE__)
#define log_error FdLog(ERROR, __FILE__, __LINE__)
#define log_fatal FdLog(FATAL, __FILE__, __LINE__)

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
	FdLog(const LOG_LEVEL level, const std::string& file_name, const int line_num);
	~FdLog();
	void log();
	void generateLogSuffix(const std::string& file_name, const int line_num);
	void generateLogPrefix(const LOG_LEVEL level);

	template<class T>
	FdLog& operator<<(T content)
	{
		log_content_ += StringTraits(content);
		return *this;
	}

private:
	static std::shared_ptr<FdLoop> fd_loop_ptr_;
	static std::shared_ptr<FdOperator> fd_operator_ptr_;
	static std::mutex mtx_;
	static std::once_flag once_;

	std::string log_suffix_;
	std::string log_prefix_;
	std::string log_content_;
	LOG_LEVEL current_log_level_;
	
	static void init();
	FdLog(FdLog&) = delete;
};

void setAsyncLogLevel(LOG_LEVEL level);
void setAsyncLogPath(const std::string& path);
void setAsyncLogFileBasename(const std::string& basename);
std::string generateLogFileName();
}


#endif
