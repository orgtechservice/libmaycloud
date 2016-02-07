#ifndef NANOSOFT_ASYNCDNS_H
#define NANOSOFT_ASYNCDNS_H

#include <maycloud/asyncobject.h>
#include <maycloud/netdaemon.h>

#include <time.h>

#include <sys/epoll.h>
#include <sys/types.h>

#include <udns.h>

/**
* Асинхронный резолвер DNS
*/
class AsyncDNS: public AsyncObject
{
protected:
	NetDaemon *daemon;
	
	/**
	* Вернуть маску ожидаемых событий
	*/
	virtual uint32_t getEventsMask();
	
	/**
	* Обработчик события
	*/
	virtual void onEvent(uint32_t events);
	
	/**
	* Сигнал завершения работы
	*
	* Объект должен закрыть файловый дескриптор
	* и освободить все занимаемые ресурсы
	*/
	virtual void onTerminate();
	
	/**
	* Таймер
	*/
	static void timer(const timeval &tv, NetDaemon *data);
	
public:
	
	/**
	* Конструктор
	*/
	AsyncDNS(NetDaemon *aDaemon);
	
	/**
	* Отправить запрос на получение IPv4-адреса
	*/
	bool a4(const char *name, dns_query_a4_fn callback, void *data);
	
	/**
	* Отправить запрос на получение SRV-записи
	*/
	bool srv(const char *name, const char *service, const char *protocol, dns_query_srv_fn callback, void *data);
	
	/**
	* Деструктор
	*/
	virtual ~AsyncDNS();
};

/**
 * Глобальная переменная-ссылка на резолвер днс
 */
extern ptr<AsyncDNS> adns;

#endif // NANOSOFT_ASYNCDNS_H
