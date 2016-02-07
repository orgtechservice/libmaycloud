
#include <maycloud/controlagent.h>
#include <maycloud/logger.h>

/**
 * Конструктор агента
 */
ControlAgent::ControlAgent():
	ControlStream(-1), connection_state(NOT_CONNECTED), reconnecting(false)
{
}

/**
 * Деструктор агента
 */
ControlAgent::~ControlAgent()
{
}

/**
 * Установить имя хоста и порт контроллера (для агента)
 */
void ControlAgent::setController(const std::string hostname, int port)
{
	controller_hostname = hostname;
	controller_port = port;
}

/**
 * Резолвер хоста контроллера, запись A (IPv4)
 */
void ControlAgent::on_connect_a4(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data)
{
	struct sockaddr_in target;
	
	ControlAgent *p = static_cast<ControlAgent *>(data);
	
	if ( result && result->dnsa4_nrr >= 1 )
	{
		p->connection_state = CONNECTING;
		
		target.sin_family = AF_INET;
		target.sin_port = htons(p->controller_port);
		memcpy(&target.sin_addr, &result->dnsa4_addr[0], sizeof(result->dnsa4_addr[0]));
		memset(target.sin_zero, 0, 8);
		
		if ( ! p->enableObject() )
		{
			// пичаль, попробуем в другой раз
			logger.unexpected("ControlAgent::startConnection() enableObject failed");
			p->reconnect();
			return;
		}
		
		if ( ::connect(p->getFd(), (struct sockaddr *)&target, sizeof( struct sockaddr )) == 0 || errno == EINPROGRESS || errno == EALREADY )
		{
			p->connection_state = CONNECTED;
			
			p->onControllerConnected();
			return;
		}
		
		printf("ControlAgent::on_connect_a4() connect fault: %s\n", strerror(errno));
		logger.error("connection failed");
		p->reconnect();
	}
	else
	{
		printf("ControlAgent::on_connect_a4() connect failed: no IP address\n");
		logger.error("connection failed: no IP address");
		p->reconnect();
	}
	
	p->disableObject();
}

/**
 * Подключиться к контроллеру
 */
void ControlAgent::startConnection()
{
	printf("ControlAgent::startConnection(%s:%d)\n", controller_hostname.c_str(), controller_port);
	
	// создадим новый сокет
	int sock = ::socket(PF_INET, SOCK_STREAM, 0);
	if ( sock == -1 )
	{
		// пичаль, попробуем в другой раз
		logger.unexpected("ControlAgent::startConnection() cannot create socket");
		reconnect();
		return;
	}
	
	setFd( sock );
	
	// Резолвим DNS записи сервера
	connection_state = RESOLVING;
	adns->a4(controller_hostname.c_str(), on_connect_a4, this);
}

/**
* Запустить процедуру переподключения
*/
void ControlAgent::reconnect()
{
	if ( ! reconnecting )
	{
		reconnecting = true;
		if ( connection_state == CONNECTED )
		{
			onControllerDisconnected();
		}
		connection_state = RECONNECTING;
		
		// закрываем старый сокет
		close();
		
		// сброс парсера
		resetParser();
		
		// сброс потока
		resetStream();
		
		// сброс билдера
		depth = 0;
		builder.reset();
		
		static unsigned int count = 1;
		logger.information("ControlAgent::reconnect() setTimeout to reconnect(%d)", count++);
		getDaemon()->setTimer(time(0) + 2, reconnectTimer, this);
	}
}

/**
* Тайкмер переподключения
*/
void ControlAgent::reconnectTimer(const timeval &tv, ControlAgent *agent)
{
	// подготовка к переподключению завершена, возвращаемся к обычному состоянию
	agent->reconnecting = false;
	
	// запустить новое подключение
	agent->startConnection();
}

/**
* Событие закрытия исходящего потока,
* когда мы сказали свое последнее слово
*/
void ControlAgent::onStreamClosed()
{
	reconnect();
}

/**
* Обработчик ошибки в асинхронном потоке
*
* @param err код ошибки errno
*/
void ControlAgent::onAsyncError(int err)
{
	logger.error("ControlAgent::onAsyncError() error[%d]: %s", err, strerror(err));
	disableObject();
	reconnect();
}

/**
* Пир (peer) закрыл поток.
*
* Мы уже ничего не можем отправить в ответ,
* можем только корректно закрыть соединение с нашей стороны.
*/
void ControlAgent::onPeerDown()
{
	logger.error("ControlAgent::onPeerDown()");
	AsyncXMLStream::onPeerDown();
	disableObject();
	reconnect();
}
