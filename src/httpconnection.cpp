
#include <maycloud/httpconnection.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpConnection::HttpConnection(int fd, AsyncWebServer *server): AsyncStream(fd) {
	this->server = server;
	request = new HttpRequest();
	response = new HttpResponse();
}

/**
 * Деструктор
 */
HttpConnection::~HttpConnection() {
	// Удаляем созданные ранее объекты запроса и ответа
	delete response;
	delete request;

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
	request->feed(buf);
	if(request->ready()) {
		server->handleRequest(request, response);
		sendResponse();
	}
}

/**
 * Отправить клиенту сформированный хэндлером HTTP-ответ
 */
void HttpConnection::sendResponse() {
	std::cout << "[AsyncWebServer::sendResponse] sending response" << std::endl;
	std::string raw_response = response->toString();
	//putInBuffer(raw_response.c_str(), raw_response.length());
	put(raw_response.c_str(), raw_response.length()); // STUB

	//shutdown(2);
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
