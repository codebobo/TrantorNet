#ifndef TCPACCEPTOR_H_
#define TCPACCEPTOR_H_

#include <functional>
#include <memory>
#include "SocketOperator.h"
#include "TrantorPipe.h"
#include "StringBuffer.h"
#include "TrantorLoop.h"
#include "TcpConnection.h"
#include "TrantorLoopThreadPool.h"

using namespace trantor;

class TcpAcceptor:public std::enable_shared_from_this<TcpAcceptor>
{
	typedef std::function<void(std::shared_ptr<TcpConnection>&)> ReadCallback;
	public:
		TcpAcceptor(TrantorLoop* main_loop_ptr);
		int init(const std::string& sever_addr, const int server_port);
		void setReadCallback(ReadCallback cb)
		{
			readCallback_ = cb;
		}
		void enableRead()
		{
			trantor_pipe_ptr_->enableRead();
		}
		
		void unregisterFromLoop()
		{
			main_loop_ptr_->removePipe(trantor_pipe_ptr_);
			//event_handler_ptr_->unregisterEvent();
		}
		void registerIntoLoop()
		{
			main_loop_ptr_->registerPipe(trantor_pipe_ptr_);
			//event_handler_ptr_->registerEvent();
		}
		void setWorkLoopPool(TrantorLoopThreadPool* work_thread_pool)
		{
			
			work_thread_pool_ = work_thread_pool;
		}

	private:
		SocketOperator socket_operator_;
		std::shared_ptr<TrantorPipe> trantor_pipe_ptr_;
		ReadCallback readCallback_;
		TrantorLoop* main_loop_ptr_;
		TrantorLoopThreadPool* work_thread_pool_;
		int listener_;

		void readSocket();
};

#endif

