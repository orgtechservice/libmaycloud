
#include <maycloud/httpconnection.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpConnection::HttpConnection(int fd, AsyncWebServer *server): AsyncStream(fd) {
	//std::cout << "[HttpConnection::HttpConnection] New incoming connection" << std::endl;
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

	// Закрываем сокет если ещё открыт
	close();

	//std::cout << "[HttpConnection::~HttpConnection] Connection closed" << std::endl;
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
		if(_response->ready()) sendResponse();
	}
}

/**
 * Отправить клиенту сформированный хэндлером HTTP-ответ
 */
void HttpConnection::sendResponse() {
	//std::cout << "[AsyncWebServer::sendResponse] sending response of type <" << _response->contentType() << ">" << std::endl;
	std::string raw_response = _response->toString();
	put(raw_response.c_str(), raw_response.length());
	_server->logRequest(_request, _response);
}

/**
 * Отчитаться о самостоятельной отправке отложенного ответа
 */
void HttpConnection::sentResponse(const std::string &content_type) {
	//std::cout << "[AsyncWebServer::sentResponse] sent response of type <" << content_type << ">" << std::endl;
	_server->logRequest(_request, _response);
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
 * Если у нас всё — просто закрываем соединение;
 * если нет — делаем что-нибудь следующее, например, отправляем следующий кусок
 */
void HttpConnection::onEmpty() {
	if(_response->pending()) {
		_response->handlePendingOperation();
	} else {
		delete this;
	}
}

/**
* Обработчик ошибки в асинхронном потоке
* @param err код ошибки errno
*/
void HttpConnection::onAsyncError(int err) {
	disableObject();
}
