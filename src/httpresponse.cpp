
#include <maycloud/httpresponse.h>

typedef std::pair<std::string, std::string> status_pair_t;

static std::map<int, std::pair<std::string, std::string>> status_map = {
	{200, status_pair_t("OK", "OK")},
	{400, status_pair_t("Bad Request", "The server was unable to parse your request.")},
	{401, status_pair_t("Authorization Required", "You should provide valid username and password to access the requested page.")},
	{403, status_pair_t("Forbidden", "You don’t have permission to browse this page.")},
	{404, status_pair_t("Object Not Found", "The requested web page does not exist within the server.")},
	{405, status_pair_t("Method Not Allowed", "The requested web page cannot be requested using the chosen method.")},
	{410, status_pair_t("Gone", "The requested web page is not available anymore.")},
	{411, status_pair_t("Length Required", "The requested web page requires Content-Length to be set.")},
	{415, status_pair_t("Unsupported Media Type", "The server does not support the provided data format.")},
	{501, status_pair_t("Not Implemented", "Something important is not implemented yet.")},
	{503, status_pair_t("Service Unavailable", "The server is temporarily unavailable.")}
};


HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	_headers["Content-Type"] = "text/html;charset=utf-8";
	_headers["Connection"] = "close";
	_headers["Server"] = connection->server()->serverIdString();
	_status = 200;

	setContentType("text/plain");
	_waiting = 0; // по умолчанию — без отложенного IO
	_sending = 0; // по умолчанию слалка не задействована
}

HttpResponse::~HttpResponse() {
	// Удалить ждалку и обязательно удалить таймеры
	if(_waiting) {
		_connection->server()->daemon()->removeTimer(_waiting->timer_id);
		delete _waiting;
		_waiting = 0;
	}

	// Аналогично со слалкой
	if(_sending) {
		delete _sending;
		_sending = 0;
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
	auto it = status_map.find(code);
	if(it != status_map.end()) {
		setSimpleHtmlPage(it->second.first + "(" + std::to_string(code) + ")", it->second.second);
	}
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
	result += "\r\n";
	result += _body;
	return result;
}

/**
 * Строка заголовков для отложенной отправки файлов
 */
std::string HttpResponse::headersString(unsigned long long content_length) {
	_headers["Content-Length"] = std::to_string(content_length);
	std::string result("");
	result += std::string("HTTP/1.1 ") + std::to_string((unsigned long long) _status) + std::string(" ") + statusText() + std::string("\r\n");
	for(auto it = _headers.begin(); it != _headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\r\n");
	}
	result += "\r\n";
	return result;
}

/**
 * Получить текущий статусный текст по текущему коду состояния HTTP
 */
std::string HttpResponse::statusText() {
	auto it = status_map.find(_status);
	if(it != status_map.end()) {
		return it->second.first;
	}
	return "";
}

/**
 * Получить статусный текст по коду состояния HTTP
 */
std::string HttpResponse::statusText(int code) {
	auto it = status_map.find(code);
	if(it != status_map.end()) {
		return it->second.first;
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
	if(_waiting || _sending) return;
	if(!fileExists(filename)) {
		setStatusPage(404);
		return;
	}

	std::regex re { R"(^(.*)\.([a-zA-Z0-9]+)$)" };
	std::smatch matches;
	if(std::regex_match(filename, matches, re)) {
		std::string content_type = mimeTypeByExtension(matches[2]);
		setContentType(content_type);
	}

	_sending = new sendfile_info_t;
	_sending->file = fopen(filename.c_str(), "r");
	fseek(_sending->file, 0, SEEK_END);
	_sending->filesize = ftell(_sending->file);
	fseek(_sending->file, 0, SEEK_SET);
	_sending->position = 0;
	_sending->content_type = _content_type;

	std::string headers(headersString(_sending->filesize));
	_connection->put(headers.c_str(), headers.length());
}

void HttpResponse::sendSmallFile(const std::string &filename) {
	if(_waiting || _sending) return;
	if(!fileExists(filename)) {
		setStatusPage(404);
		return;
	}

	std::regex re { R"(^(.*)\.([a-zA-Z0-9]+)$)" };
	std::smatch matches;
	if(std::regex_match(filename, matches, re)) {
		std::string content_type = mimeTypeByExtension(matches[2]);
		setContentType(content_type);
	}

	std::ifstream f(filename);
	setBody(std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>()));
}

void HttpResponse::handlePendingOperation() {
	if(_waiting != NULL) return;
	if(_sending == NULL) return;

	char buf[FD_READ_CHUNK_SIZE];
	size_t bytes = ::fread(&buf, sizeof(char), FD_READ_CHUNK_SIZE, _sending->file);
	if(::feof(_sending->file)) {
		::fclose(_sending->file);
		_connection->sentResponse(_sending->content_type);
		delete _sending;
		_sending = 0;
	} else {
		_sending->position += bytes;
	}

	_connection->put(buf, bytes);
}

bool HttpResponse::ready() {
	return (_waiting == 0) && (_sending == 0);
}

void HttpResponse::updateFileWaiting(const timeval &tv, void *waiting) {
	// Испражнись с того света обратно
	waiting_info_t *w = (waiting_info_t *) waiting;

	// если всё нормально
	std::ifstream input(w->filename_to_wait);
	if(input.good() || (tv.tv_sec > w->expires)) {
		input.close();
		w->handler(w->response, w->handler_userdata);
		w->response->connection()->sendResponse();
	} else {
		time_t when = time(NULL) + 1;
		w->timer_id = w->response->connection()->server()->daemon()->setTimer(when, updateFileWaiting, waiting);
	}
}

void HttpResponse::waitForFile(const std::string &filename, response_handler_t handler, uint8_t timeout, void *userdata) {
	if(_waiting || _sending) return;

	// Может ничего и не надо делать
	std::ifstream input(filename);
	if(input.good()) {
		input.close();
		handler(this, userdata);
		return;
	}

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
	_waiting->timer_id = _connection->server()->daemon()->setTimer(when, updateFileWaiting, (void *) _waiting);
}

void HttpResponse::updateFunctionWaiting(const timeval &tv, void *waiting) {
	// Ждалка, изрыгнись из небытия
	waiting_info_t *w = (waiting_info_t *) waiting;

	// если всё нормально
	std::ifstream input(w->filename_to_wait);
	if(w->custom_function(w->response, w->custom_function_userdata) || (tv.tv_sec > w->expires)) {
		w->handler(w->response, w->handler_userdata);
		w->response->connection()->sendResponse();
	} else {
		time_t when = time(NULL) + 1;
		w->timer_id = w->response->connection()->server()->daemon()->setTimer(when, updateFunctionWaiting, waiting);
	}
}

void HttpResponse::waitForFunction(custom_function_t custom_function, response_handler_t handler, uint8_t timeout, void *custom_function_userdata, void *handler_userdata) {
	if(_waiting || _sending) return;

	// Может ничего и не надо делать
	if(custom_function(this, custom_function_userdata)) {
		handler(this, handler_userdata);
		return;
	}

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
	_waiting->timer_id = _connection->server()->daemon()->setTimer(when, updateFunctionWaiting, (void *) _waiting);
}
