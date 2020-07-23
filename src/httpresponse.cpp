
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	headers["Content-Type"] = "text/html;charset=utf-8";
	headers["Connection"] = "close";
	headers["Server"] = connection->server()->serverIdString();
	_status = 200;
	status_map[200] = "OK";
	status_map[404] = "Object Not Found";
	status_map[400] = "Bad Request";
	status_map[403] = "Forbidden";
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

void HttpResponse::setBody(const std::string &body) {
	this->body = body;
}

std::string HttpResponse::toString() {
	std::string result("");
	result += std::string("HTTP/1.1 ") + std::to_string(_status) + std::string(" ") + statusText() + std::string("\n");
	for(auto it = headers.begin(); it != headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\n");
	}
	result += "\n";
	result += body;
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
