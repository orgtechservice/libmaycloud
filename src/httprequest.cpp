
#include <maycloud/httprequest.h>

/**
 * Конструктор HTTP-запроса на базе клиентского сокета
 */
HttpRequest::HttpRequest(int fd): AsyncObject(fd) {

}

/**
 * Деструктор
 */
HttpRequest::~HttpRequest() {

}

/**
 * Вернуть маску ожидаемых событий
 */
uint32_t HttpRequest::getEventsMask() {
	return EPOLLIN | EPOLLRDHUP | EPOLLONESHOT | EPOLLHUP | EPOLLERR;
}

/**
 * Обработчик события
 */
void HttpRequest::onEvent(uint32_t events) {
	if(events & EPOLLERR) {
		//handleError();
		return;
	}
	//if ( events & EPOLLIN ) handleRead();
	//if ( events & EPOLLOUT ) handleWrite();
	//if ( (events & EPOLLRDHUP) || (events & EPOLLHUP) ) handlePeerDown();
}
