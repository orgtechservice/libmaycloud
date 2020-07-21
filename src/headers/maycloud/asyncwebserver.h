#ifndef ASYNCWEBSERVER_H
#define ASYNCWEBSERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#include <maycloud/netdaemon.h>
#include <maycloud/asyncserver.h>
#include <maycloud/logger.h>
#include <maycloud/httprequest.h>

/**
* Базовый класс для сервера HTTP
*/
class AsyncWebServer: public AsyncServer
{
protected:
	/**
	 * Принять входящее соединение
	 */
	virtual void onAccept();

public:
	/**
	* Конструктор
	*/
	AsyncWebServer();
	
	/**
	* Деструктор
	*/
	~AsyncWebServer();
	
	/**
	* Подключиться к порту
	*/
	bool bind(int port);
	
	/**
	* Подключиться к IP и порту
	*/
	bool bind(const char *ip, const char *port);

	/**
	* Закрыть сокет
	*/
	void close();
};

#endif // ASYNCWEBSERVER_H
