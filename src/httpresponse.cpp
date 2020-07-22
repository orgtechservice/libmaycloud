
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	headers["Content-Type"] = "text/html;charset=utf-8";
	headers["Connection"] = "close";
	headers["Server"] = connection->server()->serverIdString();
	_status = 200;
}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setContentType(const std::string &content_type) {
	headers["Content-Type"] = content_type;
}

void HttpResponse::setStatus(int status) {
	_status = status;
}

void HttpResponse::setBody(const std::string &body) {
	this->body = body;
}

std::string HttpResponse::toString() {
	std::string result("");
	result += std::string("HTTP/1.1 ") + std::to_string(_status) + std::string(" ") + textByCode(_status) + std::string("\n");
	for(auto it = headers.begin(); it != headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\n");
	}
	result += "\n";
	result += body;
	return result;
}

std::string HttpResponse::textByCode(int status) {
	std::string result("");
	switch(status) {
		case 200: result = "OK"; break;
		case 404: result = "Object Not Found"; break;
		case 400: result = "Bad Request"; break;
	}
	return result;
}
