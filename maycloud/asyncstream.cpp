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
#ifdef HAVE_LIBZ
	compression = false;
#endif // HAVE_LIBZ

#ifdef HAVE_GNUTLS
	tls_status = tls_off;
#endif // HAVE_GNUTLS
}

/**
* Деструктор
*/
AsyncStream::~AsyncStream()
{
	disableCompression();
	disableTLS();
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
	
#ifdef HAVE_GNUTLS
	if ( tls_status == tls_handshake )
	{
		int ret = gnutls_handshake(tls_session);
		if ( ret == GNUTLS_E_SUCCESS )
		{
			tls_status = tls_on;
			printf("AsyncStream[%d]: gnutls_handshake ok\n", getFd());
		}
		else
		{
			if ( gnutls_error_is_fatal(ret) )
			{
				fprintf(stderr, "AsyncStream[%d]: gnutls_handshake fatal error: %s\n", getFd(), gnutls_strerror(ret));
				terminate();
				return;
			}
			return;
		}
	}
	
	if ( tls_status == tls_on )
	{
		ret = gnutls_record_recv(tls_session, chunk, sizeof(chunk));
		while ( ret > 0 )
		{
			putInDecompressor(chunk, ret);
			ret = gnutls_record_recv(tls_session, chunk, sizeof(chunk));
		}
		if ( ret == GNUTLS_E_AGAIN ) return;
		if ( ret == GNUTLS_E_REHANDSHAKE )
		{
			tls_status = tls_handshake;
			return;
		}
		if ( gnutls_error_is_fatal(ret) )
		{
			fprintf(stderr, "AsyncStream[%d]: gnutls_record_recv fatal error: %s\n", getFd(), gnutls_strerror(ret));
			terminate();
			return;
		}
		onError(gnutls_strerror(ret));
		return;
	}
#endif // HAVE_GNUTLS
	
	ret = ::read(getFd(), chunk, sizeof(chunk));
	while ( ret > 0 )
	{
		putInDecompressor(chunk, ret);
		ret = ::read(getFd(), chunk, sizeof(chunk));
	}
	if ( ret < 0 )
	{
		if ( errno != EAGAIN ) stderror();
	}
}

/**
* Передать полученные данные в декомпрессор
*
* Если компрессия поддерживается и включена, то данные распаковываются
* и передаются нижележащему уровню
*/
void AsyncStream::putInDecompressor(const char *data, size_t len)
{
#ifdef HAVE_LIBZ
	if ( compression )
	{
		handleInflate(data, len);
		return;
	}
#endif // HAVE_LIBZ
	
	putInReadEvent(data, len);
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

#ifdef HAVE_LIBZ
/**
* Обработка поступивших сжатых данных
*/
void AsyncStream::handleInflate(const char *data, size_t len)
{
	char buf[ZLIB_INFLATE_CHUNK_SIZE];
	
	strm_rx.next_in = (unsigned char*)data;
	strm_rx.avail_in = len;
	
	while ( strm_rx.avail_out == 0 )
	{
		strm_rx.next_out = (unsigned char*)buf;
		strm_rx.avail_out = sizeof(buf);
		
		inflate(&strm_rx, Z_SYNC_FLUSH);
		
		size_t have = sizeof(buf) - strm_rx.avail_out;
		
		putInReadEvent(buf, have);
	}
	
	strm_rx.avail_out = 0;
}
#endif // HAVE_LIBZ

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
	int len = asprintf(&buf, "AsyncStream::onAsyncError(%d) %s\n", err, strerror(err));
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
	
	int r = ::connect(getFd(), (struct sockaddr *)sa, sa_len);
	if ( r == 0 || errno == EINPROGRESS )
	{
		onConnect();
		return true;
	}
	else
	{
		onConnectFault(errno);
		return false;
	}
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
* Проверить поддерживается ли компрессия
* @return TRUE - компрессия поддерживается, FALSE - компрессия не поддерживается
*/
bool AsyncStream::canCompression()
{
#ifdef HAVE_LIBZ
	return true;
#else
	return false;
#endif // HAVE_LIBZ
}

/**
* Проверить поддерживается ли компрессия конкретным методом
* @param method метод компрессии
* @return TRUE - компрессия поддерживается, FALSE - компрессия не поддерживается
*/
bool AsyncStream::canCompression(const char *method)
{
#ifdef HAVE_LIBZ
	return strcmp(method, "zlib") == 0;
#else
	return false;
#endif // HAVE_LIBZ
}

/**
* Вернуть список поддерживаемых методов компрессии
*/
const compression_method_t* AsyncStream::getCompressionMethods()
{
	static compression_method_t methods[] = {
#ifdef HAVE_LIBZ
		"zlib",
#endif // HAVE_LIBZ
		0
	};
	return methods;
}

/**
* Вернуть флаг компрессии
* @return TRUE - компрессия включена, FALSE - компрессия отключена
*/
bool AsyncStream::isCompressionEnable()
{
#ifdef HAVE_LIBZ
	return compression;
#else
	return false;
#endif // HAVE_LIBZ
}

/**
* Вернуть текущий метод компрессии
* @return имя метода компрессии или NULL если компрессия не включена
*/
compression_method_t AsyncStream::getCompressionMethod()
{
#ifdef HAVE_LIBZ
	return compression ? "zlib" : 0;
#else
	return 0;
#endif // HAVE_LIBZ
}

/**
* Включить компрессию
* @param method метод компрессии
* @return TRUE - компрессия включена, FALSE - компрессия не включена
*/
bool AsyncStream::enableCompression(compression_method_t method)
{
#ifdef HAVE_LIBZ
	if ( ! compression && canCompression(method) )
	{
		int status;
		
		// инициализация компрессора исходящего трафика
		memset(&strm_tx, 0, sizeof(strm_tx));
		status = deflateInit(&strm_tx, ZLIB_COMPRESS_LEVEL);
		if ( status != Z_OK )
		{
			(void)deflateEnd(&strm_tx);
			return false;
		}
		
		// инициализация декомпрессора входящего трафика
		memset(&strm_rx, 0, sizeof(strm_rx));
		status = inflateInit(&strm_rx);
		if ( status != Z_OK )
		{
			(void)deflateEnd(&strm_tx);
			(void)inflateEnd(&strm_rx);
			return false;
		}
		
		compression = true;
		return true;
	}
	return false;
#else
	return false;
#endif // HAVE_LIBZ
}

/**
* Отключить компрессию
* @return TRUE - компрессия отключена, FALSE - произошла ошибка
*/
bool AsyncStream::disableCompression()
{
#ifdef HAVE_LIBZ
	if ( compression )
	{
		(void)deflateEnd(&strm_tx);
		(void)inflateEnd(&strm_rx);
		
		compression = false;
	}
#else
	return true;
#endif // HAVE_LIBZ
}

/**
* Проверить поддерживается ли TLS
* @return TRUE - TLS поддерживается, FALSE - TLS не поддерживается
*/
bool AsyncStream::canTLS()
{
#ifdef HAVE_GNUTLS
	return true;
#else
	return false;
#endif // HAVE_GNUTLS
}

/**
* Вернуть флаг TLS
* @return TRUE - TLS включен, FALSE - TLS отключен
*/
bool AsyncStream::isTLSEnable()
{
#ifdef HAVE_GNUTLS
	return tls_status != tls_off;
#else
	return false;
#endif // HAVE_GNUTLS
}

/**
* Включить TLS
* @param ctx контекст TLS
* @return TRUE - TLS включен, FALSE - произошла ошибка
*/
bool AsyncStream::enableTLS(AsyncStream::tls_ctx *ctx)
{
#ifdef HAVE_GNUTLS
	if ( tls_status != tls_off ) return true;
	
	gnutls_init(&tls_session, GNUTLS_SERVER);
	
	gnutls_priority_set (tls_session, ctx->priority_cache);
	gnutls_credentials_set(tls_session, GNUTLS_CRD_CERTIFICATE, ctx->x509_cred);
	gnutls_certificate_server_set_request(tls_session, GNUTLS_CERT_IGNORE);
	
	gnutls_transport_set_push_function(tls_session, tls_push);
	gnutls_transport_set_pull_function(tls_session, tls_pull);
	gnutls_transport_set_ptr(tls_session, static_cast<gnutls_transport_ptr_t>(this));
	
	tls_status = tls_handshake;
	return true;
#else
	return false;
#endif // HAVE_GNUTLS
}

/**
* Отключить TLS
* @return TRUE - TLS отключен, FALSE - произошла ошибка
*/
bool AsyncStream::disableTLS()
{
#ifdef HAVE_GNUTLS
	printf("AsyncStream[%d] enter disableTLS\n", getFd());
	int ret;
	if ( tls_status == tls_off )
	{
		return false;
	}
	if ( tls_status == tls_on )
	{
		printf("AsyncStream[%d]::gnutls_bye\n", getFd());
		ret = gnutls_bye(tls_session, GNUTLS_SHUT_RDWR);
		if ( ret < 0 ) onError(gnutls_strerror(ret));
	}
	printf("AsyncStream[%d]::gnutls_deinit\n", getFd());
	gnutls_deinit(tls_session);
	tls_status = tls_off;
	printf("AsyncStream[%d] leave disableTLS\n", getFd());
#endif // HAVE_GNUTLS
	return true;
}

#ifdef HAVE_GNUTLS
/**
* Push (write) function для GnuTLS
*/
ssize_t AsyncStream::tls_push(gnutls_transport_ptr_t ptr, const void *data, size_t len)
{
	AsyncStream *s = static_cast<AsyncStream*>(ptr);
	if ( s->putInBuffer(static_cast<const char *>(data), len) ) return len;
	gnutls_transport_set_errno(s->tls_session, EAGAIN);
	return -1;
}
#endif // HAVE_GNUTLS

#ifdef HAVE_GNUTLS
/**
* Pull (read) function для GnuTLS
*/
ssize_t AsyncStream::tls_pull(gnutls_transport_ptr_t ptr, void *data, size_t len)
{
	AsyncStream *s = static_cast<AsyncStream*>(ptr);
	return ::read(s->getFd(), data, len);
}
#endif // HAVE_GNUTLS

#ifdef HAVE_LIBZ
/**
* Записать данные со сжатием zlib deflate
*
* Данные сжимаются и записываются в файловый буфер. Данная функция
* пытается принять все данные и возвращает TRUE если это удалось.
*
* TODO В случае неудачи пока не возможно определить какая часть данных
* была записана, а какая утеряна.
*
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE произошла ошибка
*/
bool AsyncStream::putDeflate(const char *data, size_t len)
{
	char buf[ZLIB_DEFLATE_CHUNK_SIZE];
	
	strm_tx.next_in = (unsigned char*)data;
	strm_tx.avail_in = len;
	
	while ( strm_tx.avail_out == 0 )
	{
		strm_tx.next_out = (unsigned char*)buf;
		strm_tx.avail_out = sizeof(buf);
		
		deflate(&strm_tx, Z_PARTIAL_FLUSH);
		
		size_t have = sizeof(buf) - strm_tx.avail_out;
		
		if ( ! putInTLS(buf, have) ) return false;
	}
	
	strm_tx.avail_out = 0;
	
	return true;
}
#endif // HAVE_LIBZ

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
	
	return putInCompressor(data, len);
}

/**
* Передать данные компрессору
*
* Если сжатие поддерживается и включено, то сжать данные
* и передать на нижележащий уровень (TLS).
*
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool AsyncStream::putInCompressor(const char *data, size_t len)
{
#ifdef HAVE_LIBZ
	if ( compression )
	{
		return putDeflate(data, len);
	}
#endif // HAVE_LIBZ
	return putInTLS(data, len);
}

/**
* Передать данные в TLS
*
* Если TLS поддерживается и включено, то данные шифруются
* и передаются на нижележащий уровень (socket)
*
* @param data указатель на данные
* @param len размер данных
* @return TRUE данные приняты, FALSE данные не приняты - нет места
*/
bool AsyncStream::putInTLS(const char *data, size_t len)
{
#ifdef HAVE_GNUTLS
	if ( tls_status == tls_on )
	{
		while ( len > 0 )
		{
			ssize_t ret = gnutls_record_send(tls_session, data, len);
			if ( ret == 0 )
			{
				onError("gnutls_record_send ret=0");
				return false;
			}
			if ( ret < 0 )
			{
				onError(gnutls_strerror(ret));
				return false;
			}
			len -= ret;
			data += ret;
		}
		return true;
	}
	if ( tls_status == tls_handshake )
	{
		// нужно подождать окончания handshake
		return false;
	}
#endif // HAVE_GNUTLS
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
bool AsyncStream::shutdown(int how)
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
		int r = ::close(getFd());
		setFd(-1);
		if ( r < 0 ) stderror();
	}
}
