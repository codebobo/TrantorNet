#include "FdLog.h"

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
		}
	}
	else
	{
		abort();
	}
}

bool FdLog::setAsyncLogFilePath(const std::string& file_path)
{
	return fd_operator_ptr_->openFd(file_path, WRITE_MODE);
}

void FdLog::log(LOG_LEVEL level, const std::string& log_content)
{
	if(level <= current_log_level_)
	{
		fd_operator_ptr_->writeFd(log_content);
	}
}

void setLogLevel(LOG_LEVEL level)
{
	return FdLog::instance().setLogLevel(level);
}

bool setAsyncLogPath(const std::string& path)
{
	return FdLog::instance().setAsyncLogFilePath(path);
}


