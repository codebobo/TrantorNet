#include "FdLog.h"

namespace trantor
{
LOG_LEVEL global_log_level = DEBUG;

FdLog* FdLog::fd_log_ptr_ = NULL;
std::once_flag FdLog::once_;

FdLog::FdLog():current_log_level_(ERROR)
{
	fd_loop_ptr_ = std::make_shared<FdLoop>(10, 10);
	if(fd_loop_ptr_)
	{
		fd_operator_ptr_ = std::make_shared<FdOperator>(fd_loop_ptr_);
		if(!fd_operator_ptr_)
		{
			abort();
		}
		else
		{
			fd_operator_ptr_->setFd(STDOUT_FILENO);
			fd_operator_ptr_->registerFd();
			fd_loop_ptr_->start();
		}
	}
	else
	{
		abort();
	}
}

bool FdLog::setAsyncLogFilePath(const std::string& file_path)
{
	return fd_operator_ptr_->openFd(file_path, WRITE_APPEND);
}

void FdLog::log(const std::string& log_content)
{
	if(current_log_level_ <= global_log_level)
	{
		std::lock_guard<std::mutex> lck(mtx_);
		fd_operator_ptr_->writeFd(log_prefix_ + log_content + log_suffix_);
	}
}

void FdLog::generateLogSuffix(const std::string& file_name, const int line_num)
{
	log_suffix_ = " -- " + file_name + ":" + StringTraits(line_num) + "\n";
}
void FdLog::generateLogPrefix(const LOG_LEVEL level)
{
	log_prefix_ = TrantorTimestamp::now().transToString() + " " + StringTraits( std::this_thread::get_id()) + " ";
	switch(level)
	{
	case DEBUG:
		log_prefix_ += "DEBUG -- ";
		break;
	case INFO:
		log_prefix_ += "INFO  -- ";
		break;
	case WARN:
		log_prefix_ += "WARN  -- ";
		break;
	case ERROR:
		log_prefix_ += "ERROR -- ";
		break;
	case FATAL:
		log_prefix_ += "FATAL -- ";
		break;
	}
}

void setAsyncLogLevel(LOG_LEVEL level)
{
	global_log_level = level;
}

bool setAsyncLogPath(const std::string& path)
{
	return FdLog::instance().setAsyncLogFilePath(path);
}

FdLog& log(const LOG_LEVEL level, const std::string& file_name, const int line_num)
{
	FdLog::instance().generateLogPrefix(level);
	FdLog::instance().generateLogSuffix(file_name, line_num);
	FdLog::instance().setLogLevel(level);

	return FdLog::instance();
}

}




