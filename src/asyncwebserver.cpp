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
	http_route_map_item_t target(handler, userdata);
	get_routes[path] = target;
}

/**
 * Добавить обработчик POST-запроса
 */
void AsyncWebServer::post(const std::string &path, http_request_handler_t handler, void *userdata) {
	//std::cout << "[AsyncWebServer] STUB: register POST request handler" << std::endl;
	http_route_map_item_t target(handler, userdata);
	post_routes[path] = target;
}

/**
 * Добавить обработчик GET+POST-запроса
 */
void AsyncWebServer::route(const std::string &path, http_request_handler_t handler, void *userdata) {
	//std::cout << "[AsyncWebServer] STUB: register GP request handler" << std::endl;
	http_route_map_item_t target(handler, userdata);
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

	std::string path = request->path();
	std::string method = request->method();
	if(method == "GET") {
		auto it = get_routes.find(path);
		if(it == get_routes.end()) {
			response->setStatusPage(404);
			return;
		}
		// Вызов зарегистрированного хэндлера
		it->second.first(request, response, it->second.second);
	}
	if(method == "POST") {
		auto it = post_routes.find(path);
		if(it == post_routes.end()) {
			response->setStatusPage(405);
			return;
		}
		// Вызов зарегистрированного хэндлера
		it->second.first(request, response, it->second.second);
	}
}

/**
 * Вернуть строку идентификации сервера
 */
std::string AsyncWebServer::serverIdString() {
	return _server_id;
}
