#include "FdLog.h"

namespace trantor
{
std::string async_log_file_path_ = "";
LOG_LEVEL global_log_level = DEBUG;

std::once_flag FdLog::once_;
std::shared_ptr<FdLoop>  FdLog::fd_loop_ptr_ = NULL;
std::shared_ptr<FdOperator>  FdLog::fd_operator_ptr_ = NULL;
std::mutex  FdLog::mtx_;

FdLog::FdLog(const LOG_LEVEL level, const std::string& file_name, const int line_num)
:current_log_level_(level)
{
	std::call_once(FdLog::once_, &FdLog::init);
	generateLogPrefix(level);
	generateLogSuffix(file_name, line_num);
}

void FdLog::init()
{
	FdLog::fd_loop_ptr_ = std::make_shared<FdLoop>(10, 10);
	if(FdLog::fd_loop_ptr_)
	{
		FdLog::fd_operator_ptr_ = std::make_shared<FdOperator>(FdLog::fd_loop_ptr_);
		if(!FdLog::fd_operator_ptr_)
		{
			abort();
		}
		else
		{
			if(async_log_file_path_ == "")
			{
				FdLog::fd_operator_ptr_->setFd(STDOUT_FILENO);
			}
			else
			{
				if(!FdLog::fd_operator_ptr_->openFd(async_log_file_path_, WRITE_APPEND))
				{
					abort();
				}
			}
			FdLog::fd_operator_ptr_->registerFd();
			FdLog::fd_loop_ptr_->start();
		}
	}
	else
	{
		abort();
	}
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

void setAsyncLogPath(const std::string& path)
{
	async_log_file_path_ = path;
}
}




