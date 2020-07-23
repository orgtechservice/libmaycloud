
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	headers["Content-Type"] = "text/html;charset=utf-8";
	headers["Connection"] = "close";
	headers["Server"] = connection->server()->serverIdString();
	_status = 200;
	status_map[200] = "OK";
	status_map[400] = "Bad Request";
	status_map[401] = "Authorization Required";
	status_map[403] = "Forbidden";
	status_map[404] = "Object Not Found";
	status_map[405] = "Method Not Allowed";
	status_map[410] = "Gone";
}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setContentType(const std::string &content_type) {
	headers["Content-Type"] = content_type;
}

void HttpResponse::setStatus(int code) {
	_status = code;
}

void HttpResponse::setStatusPage(int code) {
	setStatus(code);
	if(code == 400) setSimpleHtmlPage("Bad Request (400)", "The server was unable to parse your request.");
	if(code == 401) setSimpleHtmlPage("Authorization Required (401)", "You should provide valid username and password to access the requested page.");
	if(code == 404) setSimpleHtmlPage("Not Found (404)", "The requested web page does not exist within the server.");
	if(code == 405) setSimpleHtmlPage("Method Not Allowed (405)", "The requested web page cannot be requested using the chosen method.");
}

void HttpResponse::setBody(const std::string &body) {
	_body = body;
}

std::string HttpResponse::toString() {
	headers["Content-Length"] = std::to_string((unsigned long long) _body.length());
	std::string result("");
	result += std::string("HTTP/1.1 ") + std::to_string((unsigned long long) _status) + std::string(" ") + statusText() + std::string("\r\n");
	for(auto it = headers.begin(); it != headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\r\n");
	}
	result += "\n";
	result += _body;
	return result;
}

std::string HttpResponse::statusText() {
	auto it = status_map.find(_status);
	if(it != status_map.end()) {
		return it->second;
	}
	return "";
}

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
	_body = simpleHtmlPage(title, body);
}

/**
 * Сформировать простую веб-страницу и установить её в качестве содержимого ответа
 */
void HttpResponse::requireBasicAuth(const std::string &realm) {
	headers["WWW-Authenticate"] = "Basic realm=\"" + realm + "\"";
	setStatusPage(401);
}
