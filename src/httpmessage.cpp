
#include <maycloud/httpmessage.h>

static std::map<std::string, std::string> types = {
	{"html", "text/html;charset=utf-8"},
	{"htm", "text/html;charset=utf-8"},
	{"txt", "text/plain;charset=utf-8"},
	{"m3u8", "application/vnd.apple.mpegurl"},
	{"ts", "video/mp2t"},
	{"iso", "application/octet-stream"},
	{"jpg", "image/jpeg"},
	{"png", "image/png"},
	{"mp4", "video/mp4"}
};

HttpMessage::HttpMessage(HttpConnection *connection) {
	_connection = connection;
	_content_length = 0;
}

HttpMessage::~HttpMessage() {

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

bool HttpMessage::hasLength() {
	return hasHeader("Content-Length");
}

std::map<std::string, std::string> HttpMessage::headers() {
	return _headers;
}

std::string HttpMessage::contentType() {
	return _content_type;
}

std::string HttpMessage::mimeTypeByExtension(const std::string &extension) {
	auto it = types.find(extension);
	if(it == types.end()) {
		return "text/plain";
	} else {
		return it->second;
	}
}
