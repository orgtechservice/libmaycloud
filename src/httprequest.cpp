
#include <maycloud/httprequest.h>

HttpRequest::HttpRequest(): raw_headers("") {
	got_headers = false;
	got_body = false;
}

HttpRequest::~HttpRequest() {

}

void HttpRequest::feed(const std::string &data) {
	if(!got_headers) {
		size_t pos;
		pos = data.find("\n\n");
		if(pos == std::string::npos) {
			pos = data.find("\r\n\r\n");
		}

		if(pos == std::string::npos) {
			raw_headers += data;
		} else {
			std::string last_part = data.substr(0, pos);
			raw_headers += last_part;
			got_headers = true;
			got_body = true; // STUB

			parseHeaders();
		}
	}
}

void HttpRequest::parseHeaders() {
	// STUB
}

std::string HttpRequest::path() {
	return std::string("/"); // STUB
}

/**
 * Вернуть состояние готовности HTTP-запроса (запрос полностью считан, можно отправлять ответ)
 */
bool HttpRequest::ready() {
	return got_headers && got_body;
}
