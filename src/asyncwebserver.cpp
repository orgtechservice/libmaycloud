#include <maycloud/asyncwebserver.h>

/**
* Конструктор
*/
AsyncWebServer::AsyncWebServer(NetDaemon *daemon) {
	_server_id = "libmaycloud/0.1 (github.com/orgtechservice)";
	get("/", & defaultRequestHandler, (void *) this);
}

/**
* Деструктор
*/
AsyncWebServer::~AsyncWebServer() {
	
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
}

/**
 * Добавить обработчик GET-запроса
 */
void AsyncWebServer::get(const std::string &path, http_request_handler_t handler, void *userdata) {
	//std::cout << "[AsyncWebServer] STUB: register GET request handler" << std::endl;
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;
	get_routes[path] = target;
}

/**
 * Добавить обработчик POST-запроса
 */
void AsyncWebServer::post(const std::string &path, http_request_handler_t handler, void *userdata) {
	//std::cout << "[AsyncWebServer] STUB: register POST request handler" << std::endl;
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;
	post_routes[path] = target;
}

/**
 * Добавить обработчик GET+POST-запроса
 */
void AsyncWebServer::route(const std::string &path, http_request_handler_t handler, void *userdata) {
	//std::cout << "[AsyncWebServer] STUB: register GP request handler" << std::endl;
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;
	get_routes[path] = target;
	post_routes[path] = target;
}

/**
 * Обработчик по умолчанию
 * Задаётся для корня (/) по дефолту
 */
void AsyncWebServer::defaultRequestHandler(HttpRequest *request, HttpResponse *response, void *userdata) {
	//AsyncWebServer *server = (AsyncWebServer *) userdata;
	std::string title("Hello, world!");
	std::string body("Congratulations, your AsyncWebServer is working. Now add some request handlers.");
	response->setSimpleHtmlPage(title, body);
}

/**
 * Выбрать зарегистрированный обработчик
 */
http_route_map_item_t *selectRoute(http_route_map_t *routes, const std::string &path) {
	auto pos = path.find(":");
	if(pos == std::string::npos) {
		auto it = routes->find(path);
		return &(it->second);
	}

	// Parse request mask

	return NULL;
}

/**
 * Обработать входящий HTTP-запрос,
 * То есть найти подходящий обработчик и вызвать его, скормив данные request и response.
 * Если обработчик не зарегистрирован, сформировать ошибку 404
 * @param HttpRequest* указатель на полученный HTTP-запрос
 * @param HttpResponse* указатель на формируемый ответ
 */
void AsyncWebServer::handleRequest(HttpRequest *request, HttpResponse *response) {
	std::cout << "[AsyncWebServer::handleRequest] got an incoming HTTP request" << std::endl;
	int error = request->error();
	if(error != 0) {
		response->setStatusPage(error);
		return;
	}

	std::string method = request->method();
	http_route_map_t *routes = NULL;
	if(method == "GET") {
		routes = &get_routes;
	}
	if(method == "POST") {
		routes = &post_routes;
	}

	std::string path = request->path();
	
	//auto it = routes->find(path);
	http_route_map_item_t *item = selectRoute(routes, path);
	if(!item) {
		response->setStatusPage(405);
		return;
	}

	// Вызов зарегистрированного хэндлера
	item->handler(request, response, item->userdata);
}

/**
 * Вернуть строку идентификации сервера
 */
std::string AsyncWebServer::serverIdString() {
	return _server_id;
}
