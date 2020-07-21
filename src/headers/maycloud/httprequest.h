#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>

#include <maycloud/ipv4.h>
#include <maycloud/ipv6.h>
#include <maycloud/asyncobject.h>

/**
* HTTP-запрос
*/
class HttpRequest: public AsyncObject
{
public:
    HttpRequest(int fd);
    ~HttpRequest();

    /**
	* Вернуть маску ожидаемых событий
	*/
	virtual uint32_t getEventsMask();
	
	/**
	* Обработчик события
	*/
	virtual void onEvent(uint32_t events);
};

#endif // HTTPREQUEST_H
