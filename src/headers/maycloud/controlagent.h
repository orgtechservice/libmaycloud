#ifndef CONTROLAGENT_H
#define CONTROLAGENT_H

#include <maycloud/controlstream.h>
#include <maycloud/ipv4.h>

/**
 * Базовый класс для всех агентов
 */
class ControlAgent: public ControlStream
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
	 * Имя хоста контроллера
	 */
	std::string controller_hostname;
	
	/**
	 * Порт контроллера
	 */
	int controller_port;
	
	/**
	 * Запустить процедуру переподключения
	 */
	void reconnect();
	
	/**
	 * Тайкмер переподключения
	 */
	static void reconnectTimer(const timeval &tv, ControlAgent *agent);
protected:
	/**
	 * Установить имя хоста и порт контроллера (для агента)
	 */
	void setController(const std::string hostname, int port);
	
	/**
	 * Резолвер хоста контроллера, запись A (IPv4)
	 */
	static void on_connect_a4(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data);

	/**
	 * Событие: подключение к контроллеру
	 */
	virtual void onControllerConnected() = 0;
	
	/**
	 * Событие: контроллер отключился (потеряли связь)
	 */
	virtual void onControllerDisconnected() = 0;
	
	/**
	 * Событие закрытия исходящего потока,
	 * когда мы сказали свое последнее слово
	 */
	virtual void onStreamClosed();
	
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
	ControlAgent();
	
	/**
	 * Деструктор агента
	 */
	~ControlAgent();
	
	/**
	 * Подключиться к контроллеру
	 */
	void startConnection();
};

#endif // CONTROLAGENT_H
