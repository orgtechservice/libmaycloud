
#include <maycloud/httpconnection.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpConnection::HttpConnection(int fd, AsyncWebServer *server): AsyncStream(fd) {
	request = new HttpRequest();
	response = new HttpResponse();
}

/**
 * Деструктор
 */
HttpConnection::~HttpConnection() {
	delete response;
	delete request;
}

/**
 * Обработчик прочитанных данных
 */
void HttpConnection::onRead(const char *data, size_t len) {
	std::string buf(data, len);
	request->feed(buf);

	/*
	if(!got_request_headers) {
		size_t pos = buf.find("\n\n");
		if(pos == std::string::npos) {
			request_headers += buf;
		} else {
			std::string last_part = buf.substr(0, pos);
			request_headers += last_part;
			got_request_headers = true;
			got_request_body = true; // tmp
		}
	}*/
}

/**
 * Пир (peer) закрыл поток.
 *
 * Мы уже ничего не можем отправить в ответ,
 * можем только корректно закрыть соединение с нашей стороны.
 */
void HttpConnection::onPeerDown() {

}
