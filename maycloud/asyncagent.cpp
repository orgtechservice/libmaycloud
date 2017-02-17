#include <maycloud/asyncagent.h>

/**
 * Конструктор агента
 */
AsyncAgent::AsyncAgent(std::string new_hostname, int new_port):
	AsyncStream(-1), connection_state(NOT_CONNECTED), reconnecting(false)
{
	setRemoteHost(new_hostname, new_port);
}

/**
 * Деструктор агента
 */
AsyncAgent::~AsyncAgent()
{
}

/**
 * Установить имя хоста и порт контроллера (для агента)
 */
void AsyncAgent::setRemoteHost(const std::string new_hostname, int new_port)
{
	hostname = new_hostname;
	port = new_port;
}

/**
 * Резолвер хоста контроллера, запись A (IPv4)
 */
void AsyncAgent::on_connect_a4(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data)
{
	struct sockaddr_in target;
	
	AsyncAgent *p = static_cast<AsyncAgent *>(data);
	
	if ( result && result->dnsa4_nrr >= 1 )
	{
		p->connection_state = CONNECTING;
		
		target.sin_family = AF_INET;
		target.sin_port = htons(p->port);
		memcpy(&target.sin_addr, &result->dnsa4_addr[0], sizeof(result->dnsa4_addr[0]));
		memset(target.sin_zero, 0, 8);
		
		if ( ! p->enableObject() )
		{
			// пичаль, попробуем в другой раз
			logger.unexpected("AsyncAgent::startConnection() enableObject failed");
			p->reconnect();
			return;
		}
		
		if ( ::connect(p->getFd(), (struct sockaddr *)&target, sizeof( struct sockaddr )) == 0 || errno == EINPROGRESS || errno == EALREADY )
		{
			p->connection_state = CONNECTED;
			
			p->onRemoteHostConnected();
			return;
		}
		
		printf("AsyncAgent::on_connect_a4() connect fault: %s\n", strerror(errno));
		logger.error("connection failed");
		p->reconnect();
	}
	else
	{
		printf("AsyncAgent::on_connect_a4() connect failed: no IP address\n");
		logger.error("connection failed: no IP address");
		p->reconnect();
	}
	
	p->disableObject();
}

/**
 * Подключиться к контроллеру
 */
void AsyncAgent::startConnection()
{
	printf("AsyncAgent::startConnection(%s:%d)\n", hostname.c_str(), port);
	
	// создадим новый сокет
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( sock == -1 )
	{
		// пичаль, попробуем в другой раз
		logger.unexpected("AsyncAgent::startConnection() cannot create socket");
		reconnect();
		return;
	}
	
	setFd( sock );
	
	// Резолвим DNS записи сервера
	connection_state = RESOLVING;
	adns->a4(hostname.c_str(), on_connect_a4, this);
}

/**
* Запустить процедуру переподключения
*/
void AsyncAgent::reconnect()
{
	if ( ! reconnecting )
	{
		reconnecting = true;
		if ( connection_state == CONNECTED )
		{
			onRemoteHostDisconnected();
		}
		connection_state = RECONNECTING;
		
		// закрываем старый сокет
		close();
		
		static unsigned int count = 1;
		logger.information("AsyncAgent::reconnect() setTimeout to reconnect(%d)", count++);
		getDaemon()->setTimer(time(0) + 2, reconnectTimer, this);
	}
}

/**
* Тайкмер переподключения
*/
void AsyncAgent::reconnectTimer(const timeval &tv, AsyncAgent *agent)
{
	// подготовка к переподключению завершена, возвращаемся к обычному состоянию
	agent->reconnecting = false;
	
	// запустить новое подключение
	agent->startConnection();
}

/**
 * Событие: подключение к удаленному хосту
 */
void AsyncAgent::onRemoteHostConnected()
{
	// TODO
}
	
/**
 * Событие: удаленный хост отключился (потеряли связь)
 */
void AsyncAgent::onRemoteHostDisconnected()
{
	// TODO
}

/**
 * Обработчик прочитанных данных
 */
void AsyncAgent::onRead(const char *data, size_t len)
{
	// TODO
}

/**
* Обработчик ошибки в асинхронном потоке
*
* @param err код ошибки errno
*/
void AsyncAgent::onAsyncError(int err)
{
	logger.error("AsyncAgent::onAsyncError() error[%d]: %s", err, strerror(err));
	disableObject();
	reconnect();
}

/**
* Пир (peer) закрыл поток.
*
* Мы уже ничего не можем отправить в ответ,
* можем только корректно закрыть соединение с нашей стороны.
*/
void AsyncAgent::onPeerDown()
{
	logger.error("AsyncAgent::onPeerDown()");
	disableObject();
	reconnect();
}