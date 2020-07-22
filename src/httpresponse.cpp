
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	
}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setContentType(const std::string &content_type) {

}

void HttpResponse::setStatus(unsigned short int status) {

}

void HttpResponse::setBody(const std::string &body) {
	this->body = body;
}

std::string HttpResponse::toString() {
	std::string server_id = _connection->server()->serverIdString();
	std::string result("");
	result += "HTTP/1.0 200 OK\n";
	result += "Content-Type: text/html;charset=utf-8\n";
	result += "Server: " + server_id + "\n";
	result += "\n";
	result += body;

	return result;
}
