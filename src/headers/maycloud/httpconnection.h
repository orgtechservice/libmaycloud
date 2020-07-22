#ifndef HTTP_CONNECTION_H
#define HTTP_CONNECTION_H

#include <maycloud/ipv4.h>
#include <maycloud/ipv6.h>
#include <maycloud/asyncstream.h>
#include <maycloud/httprequest.h>
#include <maycloud/httpresponse.h>

#include <string>

/**
* HTTP-запрос
*/
class HttpConnection: public AsyncStream
{
protected:
	/*std::string request_headers;
	std::string request_body;
	bool got_request_headers;
	bool got_request_body;*/
	HttpRequest *request;
	HttpResponse *response;

public:
	HttpConnection(int fd);
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
};

#endif
