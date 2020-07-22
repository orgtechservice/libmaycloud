#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <maycloud/ipv4.h>
#include <maycloud/ipv6.h>
#include <maycloud/asyncstream.h>
#include <maycloud/asyncwebserver.h>
#include <maycloud/httprequest.h>
#include <maycloud/httpresponse.h>

#include <string>

class AsyncWebServer;

/**
* HTTP-запрос
*/
class HttpConnection: public AsyncStream
{
protected:
	HttpRequest *request;
	HttpResponse *response;
	AsyncWebServer *server;

	/**
	 * Отправить клиенту сформированный хэндлером HTTP-ответ
	 */
	void sendResponse();

public:
	HttpConnection(int fd, AsyncWebServer *server);
	~HttpConnection();

	/**
	 * Обработчик прочитанных данных
	 */
	void onRead(const char *data, size_t len);

	/**
	 * Пир (peer) закрыл поток.
	 *
	 * Мы уже ничего не можем отправить в ответ,
	 * можем только корректно закрыть соединение с нашей стороны.
	 */
	void onPeerDown();

	void onEmpty();
};

#endif
