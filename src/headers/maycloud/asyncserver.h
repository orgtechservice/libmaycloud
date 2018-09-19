#ifndef NANOSOFT_ASYNCSERVER_H
#define NANOSOFT_ASYNCSERVER_H

#include <maycloud/asyncobject.h>

#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

/**
* Базовый класс для асинхронных серверов
*/
class AsyncServer: public AsyncObject
{
protected:
	/**
	* Вернуть маску ожидаемых событий
	*/
	virtual uint32_t getEventsMask();
	
	/**
	* Обработчик события
	*/
	virtual void onEvent(uint32_t events);
	
	/**
	* Принять входящее соединение
	*/
	virtual void onAccept() = 0;
	
public:
	/**
	* Конструктор
	*/
	AsyncServer();
	
	/**
	* Деструктор
	*/
	~AsyncServer();
	
	/**
	* Подключиться к порту
	*/
	bool bind(int port);
	
	/**
	* Подключиться к unix-сокету
	*/
	bool bind(const char *path);
	
	/**
	* Начать слушать сокет
	*/
	bool listen(int backlog);
	
	/**
	* Принять соединение
	*/
	int accept();
	
	/**
	* Закрыть сокет
	*/
	void close();
};

#endif // NANOSOFT_ASYNCSERVER_H
