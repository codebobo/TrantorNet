#include "TcpConnection.h"
#include "FdLog.h"
#include "TrantorTimestamp.h"

TcpConnection::TcpConnection(TrantorLoop* loop_ptr, const int fd, const std::string connection_id)
:loop_ptr_(loop_ptr), fd_(fd),connection_id_(connection_id)
{
	trantor_pipe_ptr_ = std::make_shared<TrantorPipe>(fd);
	read_buffer_ptr_ = std::make_shared<StringBuffer>();
	write_buffer_ptr_ = std::make_shared<StringBuffer>();
	if(trantor_pipe_ptr_ && read_buffer_ptr_ && write_buffer_ptr_)
	{
		trantor_pipe_ptr_->setReadCb(std::bind(&TcpConnection::readCallback, this));
		trantor_pipe_ptr_->setWriteCb(std::bind(&TcpConnection::writeCallback, this));
	}
	else
	{
		log_error<<"TcpConnection init failed";
	}
}

TcpConnection::~TcpConnection()
{
	if(fd_ > 0)
	{
		log_info<<"close fd";
		socket_operator_.close(fd_);
	}
}


void TcpConnection::write(const char* addr, const long len)
{
	write_buffer_ptr_->writeBuffer(addr,len);
	trantor_pipe_ptr_->enableWrite();
	loop_ptr_->updatePipe(trantor_pipe_ptr_);
}

void TcpConnection::readCallback()
{
	int length = socket_operator_.read(fd_, read_buffer_ptr_);
	log_info<<"read bytes: "<<length;
	if(length > 0)
	{
		if(readCallback_)
		{
			readCallback_(shared_from_this(), TrantorTimestamp::now());
		}
	}
	else
	{
		log_info<<"TcpConnection closed by peer";
		closedByPeerCallback();
	}
}

void TcpConnection::writeCallback()
{
	socket_operator_.write(fd_, write_buffer_ptr_);
	if(write_buffer_ptr_->getReadableBytes() <= 0)
	{
		trantor_pipe_ptr_->disableWrite();
		loop_ptr_->updatePipe(trantor_pipe_ptr_);
		if(shutdown_write_callback_)
		{
			shutdown_write_callback_();
			shutdown_write_callback_ = NULL;
		}
	}
}

//closed by peer, unregister event and close fd in dtor
void TcpConnection::closedByPeerCallback()
{
	loop_ptr_->removePipe(trantor_pipe_ptr_);
	if(closedCallback_)
	{
		closedCallback_(shared_from_this());
	}
}

void TcpConnection::shutdownWrite()
{
	loop_ptr_->runInLoop(std::bind(&TcpConnection::shutdownWriteInLoop, this));
}

void TcpConnection::shutdownWriteInLoop()
{
	if(write_buffer_ptr_->getReadableBytes() > 0)
	{
		shutdown_write_callback_ = [=]()
		{
			socket_operator_.shutdownWrite(fd_);
		};
	}
	else
	{
		socket_operator_.shutdownWrite(fd_);
	}
}

void TcpConnection::forceClose()
{
	loop_ptr_->runInLoop(std::bind(&TcpConnection::forceCloseInLoop, this));
}

void TcpConnection::forceCloseInLoop()
{
	if(write_buffer_ptr_->getReadableBytes() > 0)
	{
		shutdown_write_callback_ = [=]()
		{
			if(closedCallback_)
			{
				closedCallback_(shared_from_this());
			}
		};
	}
	else
	{
		if(closedCallback_)
		{
			closedCallback_(shared_from_this());
		}
	}
}





