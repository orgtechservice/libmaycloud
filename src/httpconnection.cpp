
#include <maycloud/httpconnection.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpConnection::HttpConnection(int fd, AsyncWebServer *server): AsyncStream(fd) {
	std::cout << "[HttpConnection::HttpConnection] New incoming connection" << std::endl;
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
	_request = 0;
	_response = 0;

	// Покидаем демона, чтобы гарантированно не получать от него больше вызовы
	// leaveDaemon();

	// Закрываем сокет если ещё открыт
	close();

	std::cout << "[HttpConnection::~HttpConnection] Connection closed" << std::endl;
}

/**
 * Обработчик прочитанных данных
 */
void HttpConnection::onRead(const char *data, size_t len) {
	//std::cout << "[AsyncWebServer::onRead] reading data" << std::endl;
	if(!_request) return;
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
	std::cout << "[AsyncWebServer::sendResponse] sending response of type <" << _response->contentType() << ">" << std::endl;
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

/**
 * Вернуть указатель на сервер, к которому установлено данное соединение
 */
AsyncWebServer *HttpConnection::server() {
	return _server;
}
