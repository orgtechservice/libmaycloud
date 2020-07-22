
#include <maycloud/httprequest.h>

#include <maycloud/easylib.h>

#include <algorithm>
#include <vector>
#include <iostream> // tmp

HttpRequest::HttpRequest(HttpConnection *connection): HttpMessage(connection), raw_headers(""), _method("") {
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

			bool ok = parseHeaders();
			// TODO обработка статуса парсинга
		}
	}
}

bool HttpRequest::parseHeaders() {
	//std::cout << "[HttpRequest::parseHeaders]" << std::endl;
	std::vector<std::string> supported_methods {"GET", "POST"};
	EasyVector lines = explode("\n", raw_headers);
	int count = lines.size();
	for(int i = 0; i < count; i ++) {
		std::string line = lines[i];
		if(i == 0) {
			// Строка запроса
			EasyVector parts = explode(" ", line);
			if(parts.size() == 3) {
				_method = parts[0];
				if(std::find(supported_methods.begin(), supported_methods.end(), _method) == supported_methods.end()) {
					return false; // неподдерживаемый метод
				}
				_path = parts[1];
				// parts[3] — версия протокола
			} else {
				return false;
			}
		}
		//std::cout << ' ' << lines[i] << std::endl;
	}
	return true;
}

std::string HttpRequest::path() {
	return _path;
}

/**
 * Вернуть состояние готовности HTTP-запроса (запрос полностью считан, можно отправлять ответ)
 */
bool HttpRequest::ready() {
	return got_headers && got_body;
}
