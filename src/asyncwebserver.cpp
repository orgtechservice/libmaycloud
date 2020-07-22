#include <maycloud/asyncwebserver.h>

/**
* Конструктор
*/
AsyncWebServer::AsyncWebServer() {
    get("/", & defaultRequestHandler, (void *) this);
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

	logger.information("new request from %s, port %i", ipstr, port);

	if(sock) {
		ptr<HttpConnection> client = new HttpConnection(sock, this);
		getDaemon()->addObject(client);
	}

	// TODO handle request
}

/**
 * Добавить обработчик GET-запроса
 */
void AsyncWebServer::get(const std::string &path, http_request_handler_t handler, void *userdata) {
	std::cout << "[AsyncWebServer] STUB: register request handler" << std::endl;
	http_route_map_item_t target(handler, userdata);
	routes[path] = target;
}

/**
 * Обработчик по умолчанию
 */
void AsyncWebServer::defaultRequestHandler(HttpRequest *request, HttpResponse *response, void *userdata) {
	//AsyncWebServer *server = (AsyncWebServer *) userdata;
	response->setStatus(200);
	response->setContentType("text/html;charset=utf-8");
	response->setBody("<html><body><h1>AsyncWebServer is working!</h1></body></html>\n");
}
