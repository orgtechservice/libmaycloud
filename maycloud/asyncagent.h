#ifndef NANOSOFT_ASYNCAGENT_H
#define NANOSOFT_ASYNCAGENT_H

#include <maycloud/netdaemon.h>
#include <maycloud/asyncstream.h>
#include <maycloud/asyncdns.h>
#include <maycloud/logger.h>

class AsyncAgent: public AsyncStream
{
private:
	/**
	 * Состояние подключения
	 */
	enum {
		/**
		 * Начальное состояние
		 */
		NOT_CONNECTED,
		
		/**
		 * Резолвим DNS
		 */
		RESOLVING,
		
		/**
		 * Подключаемся к серверу
		 */
		CONNECTING,
		
		/**
		 * Подключены
		 */
		CONNECTED,
		
		/**
		 * Переподключаемся
		 */
		RECONNECTING
	} connection_state;
	
	/**
	 * Флаг переподключения
	 * true - мы уже установили таймер переподключения
	 */
	bool reconnecting;
	
	/**
	 * Имя удаленного хоста
	 */
	std::string hostname;
	
	/**
	 * Порт
	 */
	int port;
	
	/**
	 * Запустить процедуру переподключения
	 */
	void reconnect();
	
	/**
	 * Таймер переподключения
	 */
	static void reconnectTimer(const timeval &tv, AsyncAgent *agent);
protected:
	/**
	 * Обработчик прочитанных данных
	 */
	virtual void onRead(const char *data, size_t len);

	/**
	 * Установить имя удаленного хоста и порт (для агента)
	 */
	void setRemoteHost(const std::string new_hostname, int new_port);
	
	/**
	 * Резолвер удаленного хоста, запись A (IPv4)
	 */
	static void on_connect_a4(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data);

	/**
	 * Событие: подключение к удаленному хосту
	 */
	virtual void onRemoteHostConnected();
	
	/**
	 * Событие: удаленный хост отключился (потеряли связь)
	 */
	virtual void onRemoteHostDisconnected();
	
	/**
	 * Обработчик ошибки в асинхронном потоке
	 *
	 * @param err код ошибки errno
	 */
	virtual void onAsyncError(int err);
	
	/**
	 * Пир (peer) закрыл поток.
	 *
	 * Мы уже ничего не можем отправить в ответ,
	 * можем только корректно закрыть соединение с нашей стороны.
	 */
	virtual void onPeerDown();
public:
	/**
	 * Конструктор агента
	 */
	AsyncAgent(std::string new_hostname, int new_port);
	
	/**
	 * Деструктор агента
	 */
	~AsyncAgent();
	
	/**
	 * Подключиться к контроллеру
	 */
	void startConnection();
};

#endif // NANOSOFT_ASYNCAGENT_H