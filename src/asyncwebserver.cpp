#include <maycloud/asyncwebserver.h>

/**
* Конструктор
*/
AsyncWebServer::AsyncWebServer(NetDaemon *daemon) {
	_daemon = daemon;
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
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;

	bool mask = (path.find("{") != std::string::npos) && (path.find("}") != std::string::npos);
	if(mask) {
		get_mask_routes[maskToRegex(path)] = target;
	} else {
		get_routes[path] = target;
	}
}

/**
 * Добавить обработчик POST-запроса
 */
void AsyncWebServer::post(const std::string &path, http_request_handler_t handler, void *userdata) {
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;

	bool mask = (path.find("{") != std::string::npos) && (path.find("}") != std::string::npos);
	if(mask) {
		post_mask_routes[maskToRegex(path)] = target;
	} else {
		post_routes[path] = target;
	}
}

/**
 * Добавить обработчик GET+POST-запроса
 */
void AsyncWebServer::route(const std::string &path, http_request_handler_t handler, void *userdata) {
	http_route_map_item_t target;
	target.handler = handler;
	target.userdata = userdata;

	bool mask = (path.find("{") != std::string::npos) && (path.find("}") != std::string::npos);

	if(mask) {
		std::string re = maskToRegex(path);
		get_mask_routes[re] = target;
		post_mask_routes[re] = target;
	} else {
		get_routes[path] = target;
		post_routes[path] = target;
	}
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
 * Преобразовать маску маршрута в регулярное выражение
 */
std::string AsyncWebServer::maskToRegex(const std::string &path) {
	std::string mask_quoted = regexEscape(path);
	std::regex needle { R"(\\\{([a-z]+)\\\})" };
	std::string result = "^" + std::regex_replace(mask_quoted, needle, "([^/]+)") + "$";
	return result;
}

/**
 * Выбрать зарегистрированный обработчик
 */
http_route_map_item_t *AsyncWebServer::selectRoute(http_route_map_t *routes, http_route_map_t *mask_routes, HttpRequest *request) {
	std::string path = request->path();
	auto routes_it = routes->find(path);
	if(routes_it != routes->end()) {
		return &(routes_it->second);
	}

	// Крайне неоптимально, но хотя бы так
	for(auto mask_routes_it = mask_routes->begin(); mask_routes_it != mask_routes->end(); ++ mask_routes_it) {
		std::smatch matches;
		if(std::regex_match(path, matches, std::regex(mask_routes_it->first))) {
			for (uint8_t i = 1; i < matches.size(); ++ i) {
				//std::cout << "[" << sm[i] << "] ";
				request->addRouteParam(i - 1, matches[i]);
			}
			return &(mask_routes_it->second);
		}
	}

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
	http_route_map_t *mask_routes = NULL;
	if(method == "GET") {
		routes = &get_routes;
		mask_routes = &get_mask_routes;
	}
	if(method == "POST") {
		routes = &post_routes;
		mask_routes = &post_mask_routes;
	}
	
	//auto it = routes->find(path);
	http_route_map_item_t *item = selectRoute(routes, mask_routes, request);
	if(!item) {
		response->setStatusPage(404);
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
