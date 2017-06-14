#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <functional>
#include "SocketOperator.h"
#include "TrantorPipe.h"
#include "TrantorLoop.h"
#include "StringBuffer.h"
#include "TrantorTimestamp.h"
#include "TrantorAny.h"
#include "FdLog.h"

using namespace trantor;

class TcpConnection: public std::enable_shared_from_this<TcpConnection>
{
	public:
		TcpConnection(TrantorLoop* loop_ptr, const int fd, const std::string connection_id);
		~TcpConnection();
		void write(const char* addr, const long len);
		void write(const std::string& str)
		{
			write(str.c_str(), str.size());
		}
		void send(const std::string& str)
		{
			write(str.c_str(), str.size());
		}
		void send(StringBuffer* buf)
		{
			write(buf->peek(), buf->getReadableBytes());
		}
		void send(const char* addr, const long len)
		{
			write(addr, len);
		}
		void enableRead()
		{
			trantor_pipe_ptr_->enableRead();
		}
		void setReadCallback(std::function<void(std::shared_ptr<TcpConnection>, const TrantorTimestamp)> cb)
		{
			readCallback_ = cb;
		}
		void setClosedCallback(std::function<void(std::shared_ptr<TcpConnection>)> cb)
		{
			closedCallback_ = cb;
		}
		
		void unregisterFromLoop()
		{
			loop_ptr_->removePipe(trantor_pipe_ptr_);
		}
		void registerIntoLoop()
		{
			loop_ptr_->registerPipe(trantor_pipe_ptr_);
		}
		void setContext(const TrantorAny& context)
  		{ 
  			context_ = context; 
		}
		TrantorAny* getMutableContext()
  		{ 
  			return &context_; 
		}
		int getFd(){return fd_;}
		const std::string getConnId(){return connection_id_;}
		std::shared_ptr<StringBuffer> getReadBufferPtr(){return read_buffer_ptr_;}

		void shutdown()
		{
			shutdownWrite();
		}

		void shutdownWrite();
		void forceClose();

	private:
		int fd_;
		SocketOperator socket_operator_;
		std::shared_ptr<TrantorPipe> trantor_pipe_ptr_;
		std::shared_ptr<StringBuffer> read_buffer_ptr_;
		std::shared_ptr<StringBuffer> write_buffer_ptr_;
		std::function<void (std::shared_ptr<TcpConnection>, const TrantorTimestamp) > readCallback_;
		std::function<void (std::shared_ptr<TcpConnection>) > closedCallback_;
		std::function<void ()> shutdown_write_callback_;
		TrantorLoop* loop_ptr_;
		std::string connection_id_;
		TrantorAny context_;

		void closedByPeerCallback();
		void writeCallback();
		void readCallback();
		void shutdownWriteInLoop();
		void forceCloseInLoop();
};

#endif
