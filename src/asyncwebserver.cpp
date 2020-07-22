#include <maycloud/asyncwebserver.h>

/**
* Конструктор
*/
AsyncWebServer::AsyncWebServer() {
    
}

/**
* Деструктор
*/
AsyncWebServer::~AsyncWebServer() {
	close();
}

void AsyncWebServer::onAccept() {
	int sock = accept();

	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;

	len = sizeof addr;
	getpeername(sock, (struct sockaddr *) &addr, &len);

	struct sockaddr_in *s = (struct sockaddr_in *) &addr;
	port = ntohs(s->sin_port);
	inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	logger.information("new request from %s", ipstr);

	if(sock) {
		ptr<HttpConnection> client = new HttpConnection(sock);
		getDaemon()->addObject(client);
	}

	// TODO handle request
}

/**
 * Добавить обработчик GET-запроса
 */
void AsyncWebServer::get(const std::string &path, http_request_handler_t handler, void *userdata) {
	std::cout << "[AsyncWebServer] STUB: register request handler" << std::endl;
}
