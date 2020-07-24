
#include <maycloud/httpmessage.h>

HttpMessage::HttpMessage(HttpConnection *connection) {
	_connection = connection;
}

HttpMessage::~HttpMessage() {

}

HttpConnection *HttpMessage::connection() {
	return _connection;
}

std::string HttpMessage::header(const std::string &header_name, const std::string &default_value) {
	auto it = _headers.find(header_name);
	if(it == _headers.end()) {
		return default_value;
	} else {
		return it->second;
	}
}

void HttpMessage::setHeader(const std::string &header_name, const std::string &value) {
	_headers[header_name] = value;
}

bool HttpMessage::hasHeader(const std::string &header_name) {
	return _headers.find(header_name) != _headers.end();
}

std::map<std::string, std::string> HttpMessage::headers() {
	return _headers;
}
