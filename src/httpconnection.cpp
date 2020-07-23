
#include <maycloud/httpconnection.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpConnection::HttpConnection(int fd, AsyncWebServer *server): AsyncStream(fd) {
	_server = server;
	_request = new HttpRequest(this);
	_response = new HttpResponse(this);
}

/**
 * Деструктор
 */
HttpConnection::~HttpConnection() {
	// Удаляем созданные ранее объекты запроса и ответа
	delete _response;
	delete _request;

	// Закрываем сокет если ещё открыт
	close();

	// Покидаем демона, чтобы гарантированно не получать от него больше вызовы
	leaveDaemon();
}

/**
 * Обработчик прочитанных данных
 */
void HttpConnection::onRead(const char *data, size_t len) {
	//std::cout << "[AsyncWebServer::onRead] reading data" << std::endl;
	std::string buf(data, len);
	_request->feed(buf);
	if(_request->ready()) {
		_server->handleRequest(_request, _response);
		sendResponse();
	}
}

/**
 * Отправить клиенту сформированный хэндлером HTTP-ответ
 */
void HttpConnection::sendResponse() {
	std::cout << "[AsyncWebServer::sendResponse] sending response" << std::endl;
	std::string raw_response = _response->toString();
	putInBuffer(raw_response.c_str(), raw_response.length());
}

/**
 * Пир (peer) закрыл поток.
 *
 * Мы уже ничего не можем отправить в ответ,
 * можем только корректно закрыть соединение с нашей стороны.
 */
void HttpConnection::onPeerDown() {
	delete this;
}

/**
 * Опустошение исходящего буфера.
 * На этом этапе все данные отправлены и необходимо закрыть соединение
 */
void HttpConnection::onEmpty() {
	delete this;
}

AsyncWebServer *HttpConnection::server() {
	return _server;
}
