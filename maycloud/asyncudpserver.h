#ifndef NANOSOFT_ASYNCUDPSERVER_H
#define NANOSOFT_ASYNCUDPSERVER_H

#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>

#include <maycloud/asyncobject.h>

/**
* Базовый класс для асинхронных серверов UDP
*/
class AsyncUDPServer: public AsyncObject
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
	* Обработать входящее сообщение
	* 
	* @param ip адрес источника
	* @param port порт источника
	* @param data пакет данных
	* @param len размер пакета данных
	*/
	virtual void onRead(const char *ip, int port, const char *data, size_t len) = 0;
	
	/**
	* Принять входящее соединение
	*/
	void handleRead();
	
public:
	/**
	* Конструктор
	*/
	AsyncUDPServer();
	
	/**
	* Деструктор
	*/
	~AsyncUDPServer();
	
	/**
	* Подключиться к порту
	*/
	bool bind(int port);
	
	/**
	* Подключиться к IP и порту
	*/
	bool bind(const char *ip, const char *port);
	
	/**
	* Подключиться к устройству
	*/
	bool bindToDevice(const char *ifname);
	
	/**
	* Установить флаг броадкаста
	*/
	bool setBroadcast(bool state);
	
	/**
	* Установить флаг Reuse addr
	*/
	bool setReuseAddr(bool state);
	
	/**
	* Закрыть сокет
	*/
	void close();
};

#endif // NANOSOFT_ASYNCUDPSERVER_H
