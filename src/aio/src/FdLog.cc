#include "FdLog.h"

namespace trantor
{
uint64_t current_log_file_size = 0;
std::string async_log_file_dir = "";
std::string async_log_file_basename = "";
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

FdLog::~FdLog()
{
	log_content_ += log_suffix_;
	log();
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
			if(async_log_file_dir == "")
			{
				FdLog::fd_operator_ptr_->setFd(STDOUT_FILENO);
			}
			else
			{
				if(!FdLog::fd_operator_ptr_->openFd(async_log_file_dir + generateLogFileName(), WRITE_APPEND))
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

void FdLog::log()
{
	if(current_log_level_ <= global_log_level)
	{
		std::lock_guard<std::mutex> lck(mtx_);
		if(async_log_file_dir == "")
		{
			//std output
			fd_operator_ptr_->writeFd(log_content_);
		}
		else
		{
			current_log_file_size += log_content_.length();
			if(current_log_file_size <= MAX_SIZE_PER_LOG_FILE)
			{
				//output to original log file
				fd_operator_ptr_->writeFd(log_content_);
			}
			else
			{
				//build new log file
				fd_operator_ptr_->closeFd();
				fd_operator_ptr_ = std::make_shared<FdOperator>(FdLog::fd_loop_ptr_);
				if(!FdLog::fd_operator_ptr_)
				{
					abort();
				}
				else
				{
					if(!FdLog::fd_operator_ptr_->openFd(async_log_file_dir + generateLogFileName(), WRITE_APPEND))
					{
						abort();
					}
				}
				FdLog::fd_operator_ptr_->registerFd();
				fd_operator_ptr_->writeFd(log_content_);
				current_log_file_size = log_content_.length();
			}
		}
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

	log_content_ = log_prefix_;
}

void setAsyncLogLevel(LOG_LEVEL level)
{
	global_log_level = level;
}

//set log file dir, if not set, using stdout. if set, ensure dir exist.
void setAsyncLogPath(std::string& dir)
{
	if(dir != "" && dir.back() != '/')
	{
		dir.push_back('/');
	}
	async_log_file_dir = dir;
}

void setAsyncLogFileBasename(const std::string& basename)
{
	async_log_file_basename = basename;
}

std::string generateLogFileName()
{
	std::string pid_str = StringTraits(getpid());
	std::string timestamp = TrantorTimestamp::now().transToString();
	if(async_log_file_basename != "")
	{
		return async_log_file_basename + "-" + pid_str + "-" + timestamp + ".log";
	}
	else
	{
		return  pid_str + "-" + timestamp + ".log";
	}
}
}




