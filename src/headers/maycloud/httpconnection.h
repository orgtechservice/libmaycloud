#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <maycloud/ipv4.h>
#include <maycloud/ipv6.h>
#include <maycloud/asyncstream.h>
#include <maycloud/asyncwebserver.h>
//#include <maycloud/httprequest.h>
//#include <maycloud/httpresponse.h>

#include <string>

/**
* HTTP-запрос
*/
class HttpConnection: public AsyncStream
{
protected:
	HttpRequest *_request;
	HttpResponse *_response;
	AsyncWebServer *_server;

public:
	HttpConnection(int fd, AsyncWebServer *server);
	~HttpConnection();

	void onAsyncError(int err);

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

	/**
	 * Отправить клиенту сформированный хэндлером HTTP-ответ
	 */
	void sendResponse();

	/**
	 * Отчитаться об уже отправленном HTTP-ответе
	 */
	void sentResponse(const std::string &content_type);

	inline AsyncWebServer *server() { return _server; }
};

#endif
