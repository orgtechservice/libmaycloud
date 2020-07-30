
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	_headers["Content-Type"] = "text/html;charset=utf-8";
	_headers["Connection"] = "close";
	_headers["Server"] = connection->server()->serverIdString();
	_status = 200;
	status_map[200] = "OK";
	status_map[400] = "Bad Request";
	status_map[401] = "Authorization Required";
	status_map[403] = "Forbidden";
	status_map[404] = "Object Not Found";
	status_map[405] = "Method Not Allowed";
	status_map[410] = "Gone";
	status_map[411] = "Length Required";
	status_map[415] = "Unsupported Media Type";
	status_map[501] = "Not Implemented";
	
	types["html"] = "text/html;charset=utf-8";
	types["htm"] = "text/html;charset=utf-8";
	types["txt"] = "text/plain;charset=utf-8";
	types["ts"] = "application/vnd.apple.mpegurl";
	types["m3u8"] = "video/mp2t";

	setContentType("text/plain");
	_waiting = 0; // по умолчанию — без отложенного IO
}

HttpResponse::~HttpResponse() {
	if(_waiting) {
		delete _waiting;
		_waiting = 0;
	}
}

/**
 * Установить MIME-тип содержимого
 */
void HttpResponse::setContentType(const std::string &content_type) {
	_content_type = content_type;
	_headers["Content-Type"] = content_type;
}

/**
 * Установить код ответа code
 */
void HttpResponse::setStatus(int code) {
	_status = code;
}

/**
 * Установить в качестве кода ответа code, а в качестве тела —
 * стандартный для данного кода документ
 */
void HttpResponse::setStatusPage(int code) {
	setStatus(code);
	if(code == 400) setSimpleHtmlPage("Bad Request (400)", "The server was unable to parse your request.");
	if(code == 401) setSimpleHtmlPage("Authorization Required (401)", "You should provide valid username and password to access the requested page.");
	if(code == 404) setSimpleHtmlPage("Not Found (404)", "The requested web page does not exist within the server.");
	if(code == 405) setSimpleHtmlPage("Method Not Allowed (405)", "The requested web page cannot be requested using the chosen method.");
	if(code == 411) setSimpleHtmlPage("Length Required (411)", "The requested web page requires Content-Length to be set.");
	if(code == 415) setSimpleHtmlPage("Unsupported Media Type (415)", "The server does not support the provided data format.");
	if(code == 501) setSimpleHtmlPage("Not Implemented (501)", "Something important is not implemented yet.");
}

void HttpResponse::setBody(const std::string &body) {
	_body = body;
	_content_length = _body.length();
}

/**
 * Сериализовать HTTP-ответ в строку
 */
std::string HttpResponse::toString() {
	_headers["Content-Length"] = std::to_string((unsigned long long) _content_length);
	std::string result("");
	result += std::string("HTTP/1.1 ") + std::to_string((unsigned long long) _status) + std::string(" ") + statusText() + std::string("\r\n");
	for(auto it = _headers.begin(); it != _headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\r\n");
	}
	result += "\n";
	result += _body;
	return result;
}

/**
 * Получить текущий статусный текст по текущему коду состояния HTTP
 */
std::string HttpResponse::statusText() {
	auto it = status_map.find(_status);
	if(it != status_map.end()) {
		return it->second;
	}
	return "";
}

/**
 * Получить статусный текст по коду состояния HTTP
 */
std::string HttpResponse::statusText(int code) {
	auto it = status_map.find(code);
	if(it != status_map.end()) {
		return it->second;
	}
	return "";
}

/**
 * Сформировать простую веб-страницу
 */
std::string HttpResponse::simpleHtmlPage(const std::string &title, const std::string &body) {
	return std::string("<html><head><title>") + title + std::string("</title></head><body><h1>")
		+ title + std::string("</h1><div>") + body + std::string("</div><hr/><a href=\"https://github.com/orgtechservice\">") + _connection->server()->serverIdString() + std::string("</a></body></html>");
}

/**
 * Сформировать простую веб-страницу и установить её в качестве содержимого ответа
 */
void HttpResponse::setSimpleHtmlPage(const std::string &title, const std::string &body) {
	setBody(simpleHtmlPage(title, body));
	setContentType("text/html;charset=utf-8");
}

/**
 * Сформировать простую веб-страницу и установить её в качестве содержимого ответа
 */
void HttpResponse::requireBasicAuth(const std::string &realm) {
	_headers["WWW-Authenticate"] = "Basic realm=\"" + realm + "\"";
	setStatusPage(401);
}

void HttpResponse::sendFile(const std::string &filename) {
	// TODO sendfile
	std::ifstream input(filename);
	if(!input.good()) {
		setStatusPage(404);
		return;
	}

	std::regex re { R"(^(.*)\.([a-zA-Z0-9]+)$)" };
	std::smatch matches;
	if(std::regex_match(filename, matches, re)) {
		std::string content_type = mimeTypeByExtension(matches[2]);
		setContentType(content_type);
		//std::cout << "CT: " << content_type << std::endl;
	}

	std::string str((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	setBody(str);
}

std::string HttpResponse::mimeTypeByExtension(const std::string &extension) {
	auto it = types.find(extension);
	if(it == types.end()) {
		return "text/plain";
	} else {
		return it->second;
	}
}

bool HttpResponse::ready() {
	return (_waiting == 0);
}

void HttpResponse::updateFileWaiting(const timeval &tv, void *waiting) {
	std::cout << "HttpResponse::updateFileWaiting()" << std::endl;

	waiting_info_t *w = (waiting_info_t *) waiting;

	// если всё нормально
	std::ifstream input(w->filename_to_wait);
	if(input.good() || (tv.tv_usec > w->expires)) {
		input.close();
		w->handler(w->response, w->handler_userdata);
		w->response->connection()->sendResponse();
	} else {
		time_t when = time(NULL) + 1;
		w->response->connection()->server()->daemon()->setTimer(when, updateFileWaiting, waiting);
	}
}

void HttpResponse::waitForFile(const std::string &filename, response_handler_t handler, uint8_t timeout, void *userdata) {
	// Текущее время
	time_t now = time(NULL);

	// Создаём ждалку отложенного IO
	_waiting = new waiting_info_t;
	_waiting->wait_type = "file";
	_waiting->handler = handler;
	_waiting->custom_function = NULL;
	_waiting->response = this;
	_waiting->filename_to_wait = filename;
	_waiting->handler_userdata = userdata;
	_waiting->custom_function_userdata = NULL;
	_waiting->expires = now + timeout;

	time_t when = now + 1;
	_connection->server()->daemon()->setTimer(when, updateFileWaiting, (void *) _waiting);
}

void HttpResponse::updateFunctionWaiting(const timeval &tv, void *waiting) {
	std::cout << "HttpResponse::updateFunctionWaiting()" << std::endl;

	// Ждалка, изрыгнись из небытия
	waiting_info_t *w = (waiting_info_t *) waiting;

	// если всё нормально
	std::ifstream input(w->filename_to_wait);
	if(w->custom_function(w->response, w->custom_function_userdata)) {
		w->handler(w->response, w->handler_userdata);
		w->response->connection()->sendResponse();
	} else {
		time_t when = time(NULL) + 1;
		w->response->connection()->server()->daemon()->setTimer(when, updateFunctionWaiting, waiting);
	}
}

void HttpResponse::waitForFunction(custom_function_t custom_function, response_handler_t handler, uint8_t timeout, void *handler_userdata, void *custom_function_userdata) {
	// Текущее время
	time_t now = time(NULL);

	// Создаём ждалку отложенного IO
	_waiting = new waiting_info_t;
	_waiting->wait_type = "function";
	_waiting->handler = handler;
	_waiting->custom_function = custom_function;
	_waiting->response = this;
	_waiting->filename_to_wait = "";
	_waiting->handler_userdata = handler_userdata;
	_waiting->custom_function_userdata = custom_function_userdata;
	_waiting->expires = now + timeout;

	time_t when = now + 1;
	_connection->server()->daemon()->setTimer(when, updateFunctionWaiting, (void *) _waiting);
}
