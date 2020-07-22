
#include <maycloud/httpresponse.h>

HttpResponse::HttpResponse(HttpConnection *connection): HttpMessage(connection) {
	std::string server_id = connection->server()->serverIdString();
	headers["Content-Type"] = "text/html;charset=utf-8";
	headers["Connection"] = "close";
	headers["Server"] = "Server: " + server_id;
}

HttpResponse::~HttpResponse() {

}

void HttpResponse::setContentType(const std::string &content_type) {
	headers["Content-Type"] = content_type;
}

void HttpResponse::setStatus(unsigned short int status) {

}

void HttpResponse::setBody(const std::string &body) {
	this->body = body;
}

std::string HttpResponse::toString() {
	std::string result("");
	result += "HTTP/1.0 200 OK\n";
	for(auto it = headers.begin(); it != headers.end(); ++ it) {
		result += (it->first + ": " + it->second + "\n");
	}
	result += "\n";
	result += body;
	return result;
}
