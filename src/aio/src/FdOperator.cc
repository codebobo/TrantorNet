#include <sys/poll.h>
#include "FdOperator.h"
#include "FdLoop.h"
//#include "TrantorConfig.h"
#include "FdOperatorManager.h"

const int FdOperator::kNoneEvent = 0;
const int FdOperator::kReadEvent = POLLIN | POLLPRI;
const int FdOperator::kWriteEvent = POLLOUT;
 
FdOperator::FdOperator(std::shared_ptr<FdLoop> loop_ptr):
events_(0), 
revents_(0),
loop_(loop_ptr), 
file_read_eof_(false),
file_write_end_(false),
registered_to_poll_(false),
file_read_length_(0),
file_read_finish_cb_(NULL),
file_write_finish_cb_(NULL),
fd_(-1)
{
}

FdOperator::~FdOperator()
{
	//close(fd_);
}

bool FdOperator::openFd (const std::string& filePath, FDMODE mode)
{
	fd_ = open(filePath.c_str(), mode);
	if(fd_ > 0)
	{
		file_path_ = filePath;
		fd_mode_ = mode;
		if(mode == WRITE_MODE || mode == WRITE_APPEND)
		{
			enableWriting();
			FdOperatorManager::instance().insertWriteFdop(filePath, shared_from_this());
		}
		else if(mode == READ_MODE)
		{
			enableReading();
			FdOperatorManager::instance().insertReadFdop(filePath, shared_from_this());
		}
		return true;
	}
	//LOG_ERROR<<"file open error"<<" filePath: "<<filePath;
	return false;
}



void FdOperator::registerFd ()
{
	registered_to_poll_ = true;
	loop_->registerFd(shared_from_this());
}

long FdOperator::seekFd (uint64_t offset, WHENCE whence)
{
	if(!registered_to_poll_)
	{
		switch(whence)
		{
		case FD_SEEK_SET:
			return lseek(fd_, offset, SEEK_SET);
		case FD_SEEK_CUR:
			return lseek(fd_, offset, SEEK_CUR);
		case FD_SEEK_END:
			return lseek(fd_, offset, SEEK_END);
		}
	}
	else
	{
		return -1;
	}
}

void FdOperator::readFd (uint64_t length, std::function<void(std::string&, bool)> fileReadFinishCb)
{
	//LOG_DEBUG<<"read fd";
	auto func = [=]()
	{
		if(this)
		{
			std::string fileContent;
			//uint64_t length1 = this->inputBuffer_.readableBytes();
			//uint64_t length2 = length1 > length ? length : length1;
			//fileContent.append(inputBuffer_.peek(), length2); 
			//this->inputBuffer_.retrieve(length2);
			uint64_t read_size = this->input_buffer_.readBuffer(fileContent, length);
			if(this->file_read_eof_)
			{
				if(this->input_buffer_.getReadableBytes() == 0)
				{
					//LOG_DEBUG<<"file read finish, content length: "<<fileContent.length();
					fileReadFinishCb(fileContent, true);
				}
				else
				{
					//LOG_DEBUG<<"file buffer to flush, content length: "<<fileContent.length();
					fileReadFinishCb(fileContent, false);
				}
			}
			else if(read_size != 0)
			{
				//LOG_DEBUG<<"file buffer data get"<<" read_size: "<<read_size<<" length: "<<length;
				fileReadFinishCb(fileContent, false);
			}
			else
			{
				//LOG_DEBUG<<"file buffer data wait"<<" read_size: "<<read_size<<" length: "<<length;
				this->file_read_length_ = length;
				this->file_read_finish_cb_ = fileReadFinishCb;
			}
			if(input_buffer_.getReadableBytes() >= BUFFER_HIGH_WATER_LEVEL_THRESHOLD)
			{
				enableReading();
			}
		}
		else
		{
			//LOG_DEBUG<<"fdoperator destroyed!";
		}
	};
	if(registered_to_poll_)
	{
		loop_->runInLoop(func);
	}
	else
	{
		func();
	}
}

void FdOperator::writeFd (const std::string& data, std::function<void(bool)> fileWriteFinishCb)
{
	//LOG_DEBUG<<"write fd";
	std::function<void()> func = [=]()
	 {
		 if(this)
		 {
			 if(this->output_buffer_.getReadableBytes() < BUFFER_HIGH_WATER_LEVEL_THRESHOLD)
			 {
				 //this->outputBuffer_.ensureWritableBytes(data.length());
				 this->output_buffer_.writeBuffer(data);
				 if(fileWriteFinishCb)
				 {
					 fileWriteFinishCb(true);
				 }
			 }
			 else
			 {
				 this->file_write_data_ = data;
				 this->file_write_finish_cb_ = fileWriteFinishCb;
			 }

			 if(this->output_buffer_.getReadableBytes() > 0)
			 {
				 enableWriting();
			 }
		}
		 else
		 {
			 if(fileWriteFinishCb)
			 {
				 fileWriteFinishCb(false);
			 }
		 }
	};
	if(registered_to_poll_)
	{
		loop_->runInLoop(func);
	}
	else
	{
		func();
	}
}

void FdOperator::closeFd ()
{
	//LOG_INFO<<"close fd";
	auto func = [=]()
	{
		//LOG_INFO<<"mode: "<<fdMode_<<"  "<<WRITE_MODE<<"  "<<WRITE_APPEND;
		if(fd_mode_ == WRITE_MODE || fd_mode_ == WRITE_APPEND)
		{
			if(this->output_buffer_.getReadableBytes() == 0)
			{
				this->clearFd();
			}
			else
			{
				//LOG_DEBUG<<"set file write finish";
				this->file_write_end_ = true;
			}
		}
		else
		{
			this->clearFd();
		}
	};
	loop_->runInLoop(func);
}

void FdOperator::clearFd()
{
	//LOG_DEBUG<<"clear fd: "<<fd_mode_;
	{
		if(fd_mode_ == READ_MODE)
		{
			FdOperatorManager::instance().eraseReadFdop(file_path_, shared_from_this());
		}
		else if(fd_mode_ == WRITE_MODE || fd_mode_ == WRITE_APPEND)
		{
			FdOperatorManager::instance().eraseWriteFdop(file_path_, shared_from_this());
		}
	}
	close(fd_);
	loop_->deleteFd(shared_from_this());
}

void FdOperator::handleEvents()
{
	if(revents_ & POLLIN)
	{
		handleRead();
	}
	if(revents_ & POLLOUT)
	{
		handleWrite();
	}
}

void FdOperator::handleRead()
{
	if(input_buffer_.getReadableBytes() < BUFFER_HIGH_WATER_LEVEL_THRESHOLD)
	{
		char tmpBuffer[BUFFER_HIGH_WATER_LEVEL_THRESHOLD];
		uint64_t n = read(fd_, tmpBuffer, BUFFER_HIGH_WATER_LEVEL_THRESHOLD);
		//int savedErrno = 0;
		//uint64_t n = inputBuffer_.readFd(fd_, &savedErrno);
		//LOG_DEBUG<<"handle read! read bytes: "<<n;
		if (n < 0)
		{
			//LOG_ERROR<<"file read error: "<<" fd: "<<fd_;
			return;
		}
		else if(n == 0)
		{
			disableReading();
			{
				if(FdOperatorManager::instance().waitToWrite(file_path_))
				{
					//LOG_DEBUG<<"wait to post file! file path: "<<file_path_;
					return;
				}
			}
			//LOG_DEBUG<<"file read eof";
			file_read_eof_ = true;
		}
		else
		{
			input_buffer_.writeBuffer(tmpBuffer, n);
			if(input_buffer_.getReadableBytes() >= BUFFER_HIGH_WATER_LEVEL_THRESHOLD)
			{
				disableReading();
			}
		}
		if(file_read_finish_cb_)
		{
			std::string fileContent;
			//uint64_t length1 = this->inputBuffer_.readableBytes();
			//uint64_t length2 = length1 > fileReadLength_ ? fileReadLength_ : length1;
			//fileContent.append(inputBuffer_.peek(), length2); 
			//this->inputBuffer_.retrieve(length2);
			input_buffer_.readBuffer(fileContent, file_read_length_);
			//LOG_DEBUG<<"file content length: "<<fileContent.length();
			file_read_finish_cb_(fileContent, file_read_eof_);
			file_read_finish_cb_ = NULL;
		}
	}
	else
	{
		//LOG_WARN<<"handle read while input buffer high water level true";
	}
}

void FdOperator::handleWrite()
{
	uint64_t n = write(fd_, output_buffer_.getReadAddr(), output_buffer_.getReadableBytes());
	//LOG_DEBUG<<"write bytes: "<<n<<" "<<output_buffer_.getReadableBytes();
	if(n > 0)
	{
		//outputBuffer_.retrieve(n);
		output_buffer_.retrieve(n);
		FdOperatorManager::instance().trigReadEvent(file_path_);
	}
	if(file_write_finish_cb_ && (output_buffer_.writeBuffer(file_write_data_) == 0))
	{
		//outputBuffer_.ensureWritableBytes(fileWriteData_.length());
		//outputBuffer_.append(fileWriteData_.c_str(), fileWriteData_.length());
		//output_buffer_.writeBuffer(file_write_data_);
		file_write_data_.resize(0);
		file_write_finish_cb_(true);
		file_write_finish_cb_ = NULL;
	}
	if(output_buffer_.getReadableBytes() == 0)
	{
		disableWriting();
		if(file_write_end_)
		{
			clearFd();
		}
	}
}




