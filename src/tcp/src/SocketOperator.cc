#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include "SocketOperator.h"
#include "FdLog.h"

int SocketOperator::createSocket()
{
	int fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0)
	{
		log_error<<"create socket failed!";
	}
	return fd;
}

int SocketOperator::bind(const int fd, const std::string& sever_addr, const int server_port)
{
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(sever_addr.c_str());
	sin.sin_port = htons(server_port);

	int ret = ::bind(fd, (const sockaddr*)&sin, sizeof(sin));

	if(ret < 0)
	{
		log_error<<"bind failed!";
	}
	return ret;
}

int SocketOperator::listen(const int fd, const int queue_length)
{
	int ret = ::listen(fd, 100);

	if(ret < 0)
	{
		log_error<<"listen failed!";
	}
	return ret;
}

int SocketOperator::accept(const int fd)
{
	int sockfd;

	struct sockaddr_in client;
	socklen_t len = sizeof(sockaddr_in);
	sockfd = ::accept(fd, (struct sockaddr*)&client, &len );
	if(sockfd >= 0)
	{
		int flags = ::fcntl(sockfd,F_GETFL,0);//��ȡ������sockfd�ĵ�ǰ״̬����������
		::fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);//����ǰsockfd����Ϊ������
	}
	else
	{
		log_error<<"accept failed!"<<sockfd<<"  "<<fd<<" "<<errno;
	}
	return sockfd;
}

void SocketOperator::write(const int fd, std::shared_ptr<StringBuffer>& buffer)
{
	int length = ::write(fd, buffer->getReadAddr(), buffer->getReadableBytes());
	buffer->retrieve(length);
}

int SocketOperator::read(const int fd, std::shared_ptr<StringBuffer>& buffer)
{
	char extrabuf[65536];
	int length = 65536;
	length = ::read(fd, extrabuf, 65536);
	if(length > 0)
	{
		log_debug<<"socket read length: "<<length ;
		buffer->writeBuffer(extrabuf,length);
	}
	return length;
}

void SocketOperator::close(const int fd)
{
	if(::close(fd) < 0)
	{
		log_error<<"socket close failed!";
	}
}

void SocketOperator::shutdownWrite(const int fd)
{
	if(::shutdown(fd, SHUT_WR) < 0)
	{
		log_error<<"socket shutdown failed!";
	}
}