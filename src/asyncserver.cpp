#include <maycloud/asyncserver.h>

using namespace std;

/**
* Конструктор
*/
AsyncServer::AsyncServer()
{
}

/**
* Деструктор
*/
AsyncServer::~AsyncServer()
{
	close();
}

/**
* Подключиться к порту
*/
bool AsyncServer::bind(int port)
{
	setFd( socket(AF_INET, SOCK_STREAM, 0) );
	if ( getFd() == 0 )
	{
		stderror();
		return false;
	}
	
	int yes = 1;
	if ( setsockopt(getFd(), SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) stderror();
	
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	
	if ( ::bind(getFd(), (sockaddr *)&addr, sizeof(addr)) != 0 )
	{
		stderror();
		close();
		return false;
	}
	
	return true;
}

/**
* Подключиться к unix-сокету
*/
bool AsyncServer::bind(const char *path)
{
	setFd( socket(AF_LOCAL, SOCK_STREAM, 0) );
	if ( getFd() == 0 )
	{
		stderror();
		return false;
	}
	
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
	unlink(path);
	if ( ::bind(getFd(), (struct sockaddr *) &addr, sizeof(addr)) != 0 )
	{
		stderror();
		close();
		return false;
	}
	
	return true;
}

/**
* Начать слушать сокет
*/
bool AsyncServer::listen(int backlog)
{
	if ( ::listen(getFd(), backlog) != 0 )
	{
		stderror();
		return false;
	}
	return true;
}

/**
* Принять соединение
*/
int AsyncServer::accept()
{
	int sock = ::accept(getFd(), 0, 0);
	if ( sock > 0 ) return sock;
	stderror();
	return 0;
}

/**
* Закрыть сокет
*/
void AsyncServer::close()
{
	if ( getFd() ) {
		int r = ::close(getFd());
		setFd(0);
		if ( r != 0 ) stderror();
	}
}


/**
* Вернуть маску ожидаемых событий
*/
uint32_t AsyncServer::getEventsMask()
{
	return EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLHUP | EPOLLERR;
}

/**
* Обработчик события
*/
void AsyncServer::onEvent(uint32_t events)
{
	if ( events & EPOLLERR ) onError("epoll report some error in stream...");
	if ( events & EPOLLIN ) onAccept();
}
