
#include <maycloud/asyncudpserver.h>
#include <maycloud/config.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdio.h>


/**
* Конструктор
*/
AsyncUDPServer::AsyncUDPServer()
{
}

/**
* Деструктор
*/
AsyncUDPServer::~AsyncUDPServer()
{
	close();
}

/**
* Подключиться к порту
*/
bool AsyncUDPServer::bind(int port)
{
	setFd( socket(PF_INET, SOCK_DGRAM, 0) );
	if ( getFd() == 0 )
	{
		stderror();
		return false;
	}
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	
	
	int status = ::bind(getFd(), (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
	if ( status == -1)
	{
		stderror();
		return false;
	}
	
	return true;
}

/**
* Подключиться к IP и порту
*/
bool AsyncUDPServer::bind(const char *ip, const char *port)
{
	struct addrinfo hints, *res;
	
	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
	getaddrinfo(ip, port, &hints, &res);
	
	setFd( socket(res->ai_family, res->ai_socktype, res->ai_protocol) );
	if ( getFd() <= 0 )
	{
		stderror();
		return false;
	}
	
	// bind it to the port we passed in to getaddrinfo():
	int status = ::bind(getFd(), res->ai_addr, res->ai_addrlen);	
	if ( status <= -1)
	{
		stderror();
		return false;
	}
	
	return true;
}

/**
* Подключиться к устройству
*/
bool AsyncUDPServer::bindToDevice(const char *ifname)
{
	return setsockopt(getFd(), SOL_SOCKET, SO_BINDTODEVICE, ifname, strlen(ifname)) == 0;
}

/**
* Установить флаг броадкаста
*/
bool AsyncUDPServer::setBroadcast(bool state)
{
	int yes = state ? 1 : 0;
	if ( setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
	{
		stderror();
		return 0;
	}
	return 1;
}

/**
* Установить флаг Reuse addr
*/
bool AsyncUDPServer::setReuseAddr(bool state)
{
	int yes = state ? 1 : 0;
	if ( setsockopt(getFd(), SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) == -1)
	{
		stderror();
		return 0;
	}
	return 1;
}

/**
* Закрыть сокет
*/
void AsyncUDPServer::close()
{
	printf("AsyncUDPServer::close()\n");
	if ( getFd() ) {
		int r = ::close(getFd());
		setFd(0);
		if ( r != 0 ) stderror();
	}
}


/**
* Вернуть маску ожидаемых событий
*/
uint32_t AsyncUDPServer::getEventsMask()
{
	return EPOLLIN | EPOLLPRI | EPOLLRDHUP | EPOLLONESHOT | EPOLLHUP | EPOLLERR;
}

/**
* Обработчик события
*/
void AsyncUDPServer::onEvent(uint32_t events)
{
	if ( events & EPOLLERR ) fprintf(stderr, "epoll report some error in stream...");
	if ( events & EPOLLIN ) handleRead();
}

/**
* Принять входящее соединение
*/
void AsyncUDPServer::handleRead()
{
	char message[FD_READ_CHUNK_SIZE];
	char ip[64];
	struct sockaddr_storage from;
	socklen_t from_len = sizeof(from);
	int port;
	
	while ( true )
	{
		memset(&from, 0, sizeof(from));
		int r = recvfrom(getFd(), &message, sizeof(message), 0, (struct sockaddr*)&from, &from_len);
		if ( r == -1 )
		{
			stderror();
			return;
		}
		
		if ( from.ss_family == AF_INET )
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in*)&from;
			inet_ntop(from.ss_family, &(ipv4->sin_addr), ip, sizeof(ip));
			port = ipv4->sin_port;
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6*)&from;
			inet_ntop(from.ss_family, &(ipv6->sin6_addr), ip, sizeof(ip));
			port = ipv6->sin6_port;
		}
		
		onRead(ip, port, message, r);
	}
}
