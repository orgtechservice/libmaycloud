#include <maycloud/asyncstream.h>
#include <maycloud/netdaemon.h>
#include <maycloud/debug.h>

#include <stdio.h>
#include <stdlib.h>

/**
* Конструктор
*/
AsyncStream::AsyncStream(int afd): AsyncObject(afd), flags(0)
{

}

/**
* Деструктор
*/
AsyncStream::~AsyncStream()
{
	close();
}

/**
* Обработка поступивших сообщений об ошибках в потоке
*/
void AsyncStream::handleError()
{
	int error = 0;
	socklen_t errlen = sizeof(error);
	int r = getsockopt(getFd(), SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
	if ( r == -1 )
	{
		fprintf(stderr, "AsyncStream::handleError() getsockopt(SO_ERROR) fault: %s\n", strerror(errno));
	}
	else
	{
		if ( errlen != sizeof(error) )
		{
			fprintf(stderr, "AsyncStream::handleError() getsockopt(SO_ERROR) unexpected optlen\n");
		}
		onAsyncError(error);
	}
}

/**
* Обработка поступивших данных
* 
* Данные читаются из сокета и при необходимости обрабатываются
* (сжатие, шифрование и т.п.). Обработанные данные затем передаются
* виртуальному методу onRead()
*/
void AsyncStream::handleRead()
{
	char chunk[FD_READ_CHUNK_SIZE];
	ssize_t ret;

	ret = ::read(getFd(), chunk, sizeof(chunk));
	while ( ret > 0 )
	{
		putInReadEvent(chunk, ret);
		ret = ::read(getFd(), chunk, sizeof(chunk));
	}
	if ( ret == 0 ) close();
	if ( ret < 0 )
	{
		if ( errno != EAGAIN ) stderror();
	}
}

/**
* Передать данные обработчику onRead()
*/
void AsyncStream::putInReadEvent(const char *data, size_t len)
{
	if ( DEBUG::DUMP_IO )
	{
		std::string io_dump(data, len);
		printf("DUMP READ[%d]: \033[22;32m%s\033[0m\n", getFd(), io_dump.c_str());
	}
	
	onRead(data, len);
}

/**
* Отправка накопленных данных
*
* Эта функция вызывается когда сокет готов принять данные и производит
* отправку данных накопленных в файловом буфере
*/
void AsyncStream::handleWrite()
{
	NetDaemon *d = getDaemon();
	if ( d )
	{
		if ( d->push(getFd()) )
		{
			onEmpty();
		}
	}
	else fprintf(stderr, "AsyncStream[%d]::handleWrite, daemon=NULL\n", getFd());
}

/**
* Обработка обрыва связи
*/
void AsyncStream::handlePeerDown()
{
	printf("AsyncStream[%d] peer down\n", getFd());
	onPeerDown();
}

/**
* Вернуть маску ожидаемых событий
*/
uint32_t AsyncStream::getEventsMask()
{
	return EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLHUP | EPOLLERR;
}

/**
* Обработчик события
*/
void AsyncStream::onEvent(uint32_t events)
{
	if ( events & EPOLLERR )
	{
		handleError();
		return;
	}
	if ( events & EPOLLIN ) handleRead();
	if ( events & EPOLLOUT ) handleWrite();
	if ( (events & EPOLLRDHUP) || (events & EPOLLHUP) ) handlePeerDown();
}

/**
* Обработчик ошибки разрешения домена
*/
void AsyncStream::onResolveFault()
{
}

/**
* Обработчик установления соединения
* 
* Этот обработчик вызывается после успешного вызова connect().
* В ассинхронном режиме это обычно происходит до полноценного
* установления соединения, но движок уже готов буферизовать данные
* и как только соединение будет завершено, данные будут отправлены.
* Если соединение установить не удасться, то будет вызван обработчик
* onError()
*/
void AsyncStream::onConnect()
{
}

/**
* Обработчик ошибки соединения
* 
* Этот обработчик вызывается после неудачного вызова connect()
* 
* @param err код ошибки errno
*/
void AsyncStream::onConnectFault(int err)
{
}

/**
* Обработчик ошибки в асинхронном потоке
*
* @param err код ошибки errno
*/
void AsyncStream::onAsyncError(int err)
{
	char *buf;
	int res __attribute__((unused));
	res = asprintf(&buf, "AsyncStream::onAsyncError(%d) %s\n", err, strerror(err));
	onError(buf);
	free(buf);
}

/**
* Обработчик события опустошения выходного буфера
*
* Вызывается после того как все данные были записаны в файл/сокет
*/
void AsyncStream::onEmpty()
{
}

/**
* Установить соединение
* @param sa указатель на структуру sockaddr
* @param sa_len длина структуры sockaddr
*/
bool AsyncStream::connectEx(void *sa, size_t sa_len)
{
	// принудительно выставить O_NONBLOCK
	int flags = fcntl(getFd(), F_GETFL, 0);
	if ( flags >= 0 )
	{
		fcntl(getFd(), F_SETFL, flags | O_NONBLOCK);
	}

	if ( ::connect(getFd(), (struct sockaddr *)sa, sa_len) == -1 )
	{
		if(errno == EINPROGRESS || errno == EALREADY)
		{
			fd_set rfds, wfds;
			FD_ZERO(&rfds);
			FD_ZERO(&wfds);
			FD_SET(getFd(), &rfds);
			FD_SET(getFd(), &wfds);
			/* Ждем не больше пяти секунд. */
			struct timeval tv;
			tv.tv_sec = 5;
			tv.tv_usec = 0;
			/* Данный вызов является блокирующим, но в данном случае для нас это не критично */
			if ( select(getFd() + 1, &rfds, &wfds, NULL, &tv) > 0 )
			{
				socklen_t len = sizeof(int);
				int optval = 0;
				getsockopt(getFd(), SOL_SOCKET, SO_ERROR, (void*)(&optval), &len); 
				if(!optval)
				{
					onConnect();
					return true;
				}
			}
		}
	}
	else
	{
		onConnect();
		return true;
	}

	onConnectFault(errno);
	return false;
}

/**
* Установить соединение с сервером
* @param ip IP-адрес хоста
* @param port порт
* @return TRUE - соединение открыто, FALSE - произошла ошибка
*/
bool AsyncStream::connectTo(unsigned int ip, unsigned int port)
{
	struct sockaddr_in target;
	memset(&target, 0, sizeof(target));
	target.sin_family = AF_INET;
	target.sin_port = htons(port);
	target.sin_addr.s_addr = htonl(ip);
	return connectEx(&target, sizeof(target));
}

/**
* Установить TCP-соединение с хостом
* @param host доменное имя или IP-адрес хоста
* @param port порт
*/
bool AsyncStream::connectTo(const char *host, unsigned int port)
{
	struct sockaddr_in target;
	memset(&target, 0, sizeof(target));
	if ( inet_pton(AF_INET, host, &(target.sin_addr)) == 1 )
	{
		target.sin_family = AF_INET;
		target.sin_port = htons(port);
		return connectEx(&target, sizeof(target));
	}
	
	// TODO
	onResolveFault();
	return false;
}

/**
* Записать данные
*
* При необходимости данные обрабатываются (сжимаются, шифруются и т.п.)
* и записываются в файловый буфер. Фактическая отправка данных будет
* когда сокет готов будет принять очередную порцию данных. Эта функция
* старается принять все переданные данные и возвращает TRUE если это удалось.
* Если принять данные не удалось, то возвращается FALSE. В случае
* использования сжатия и/или компрессии невозможно точно установить
* какая часть данных была записана в буфер.
*
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool AsyncStream::put(const char *data, size_t len)
{
	if ( DEBUG::DUMP_IO )
	{
		std::string io_dump(data, len);
		printf("DUMP WRITE[%d]: \033[22;34m%s\033[0m\n", getFd(), io_dump.c_str());
	}
	
	return putInBuffer(data, len);
}

/**
* Передать данные в файловый буфер
*
* Данные записываются сначала в файловый буфер и только потом отправляются.
* Для обеспечения целостности переданный блок либо записывается целиком
* и функция возвращает TRUE, либо ничего не записывается и функция
* возвращает FALSE
*
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool AsyncStream::putInBuffer(const char *data, size_t len)
{
	NetDaemon *daemon = getDaemon();
	if ( daemon )
	{
		if ( daemon->put(getFd(), data, len) )
		{
			daemon->modifyObject(this);
			return true;
		}
	}
	return false;
}

/**
* Завершить чтение/запись
* @note только для сокетов
*/
void AsyncStream::shutdown(int how)
{
	if ( how & READ & ~ flags ) {
		if ( ::shutdown(getFd(), SHUT_RD) != 0 ) stderror();
		flags |= READ;
	}
	if ( how & WRITE & ~ flags ) {
		if ( ::shutdown(getFd(), SHUT_WR) != 0 ) stderror();
		flags |= WRITE;
	}
}

/**
* Закрыть поток
*/
void AsyncStream::close()
{
	if ( getFd() != -1 )
	{
		disableObject();
		int r = ::close(getFd());
		setFd(-1);
		if ( r < 0 ) stderror();
	}
}
