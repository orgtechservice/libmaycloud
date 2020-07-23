
#include <maycloud/httprequest.h>

#include <maycloud/easylib.h>

#include <algorithm>
#include <vector>
#include <iostream> // tmp

HttpRequest::HttpRequest(HttpConnection *connection): HttpMessage(connection) {
	got_headers = false;
	got_body = false;
	_error = 0;
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
	//std::cout << "[HttpRequest::parseHeaders]" << std::endl;
	std::vector<std::string> supported_methods {"GET", "POST"};
	EasyVector lines = explode("\n", raw_headers);
	int count = lines.size();
	if(count < 1) {
		_error = 400;
		return;
	}

	EasyVector parts;
	parts = explode(" ", lines[0]);
	if(parts.size() == 3) {
		_method = parts[0];
		if(std::find(supported_methods.begin(), supported_methods.end(), _method) == supported_methods.end()) {
			_error = 405;
		}
		_path = parts[1];
		// parts[3] — версия протокола
	} else {
		_error = 400;
	}

	for(int i = 1; i < count; i ++) {
		parts = explode(": ", lines[i]);
		if(parts.size() == 2) {
			parseHeader(parts[0], parts[1]);
		} else {
			_error = 400;
		}
		//std::cout << ' ' << lines[i] << std::endl;
	}
}

void HttpRequest::parseHeader(const std::string &name, const std::string &value) {
	if(name == "Authorization") {
		EasyVector parts = explode(" ", value);
		if(parts.size() != 2) {
			_error = 400;
		}
		if(parts[0] != "Basic") {
			_error = 501;
		}
		handleBasicAuth(parts[1]);
	}
}

void HttpRequest::handleBasicAuth(const std::string &base64_auth) {
	std::string auth_info = base64_decode(base64_auth);
	EasyVector parts = explode(":", auth_info);
	if(parts.size() != 2) {
		_error = 400;
	}
	has_auth_info = true;
	_username = parts[0];
	_password = parts[1];
}

std::string HttpRequest::path() {
	return _path;
}

std::string HttpRequest::method() {
	return _method;
}

std::string HttpRequest::host() {
	return _host;
}

std::string HttpRequest::username() {
	return _username;
}

std::string HttpRequest::password() {
	return _password;
}

/**
 * Вернуть состояние готовности HTTP-запроса (запрос полностью считан, можно отправлять ответ)
 */
bool HttpRequest::ready() {
	return got_headers && got_body;
}

/**
 * Вернуть возможно возникшую на этапе обработки запроса ошибку (0 = всё хорошо)
 */
int HttpRequest::error() {
	return _error;
}

bool HttpRequest::hasAuthInfo() {
	return has_auth_info;
}

bool HttpRequest::authenticateUser(HttpResponse *response, const std::string &username, const std::string &password, const std::string &realm) {
	if(!has_auth_info) {
		response->requireBasicAuth(realm);
		return false;
	} else {
		if((_username == username) && (_password == password)) {
			return true;
		} else {
			response->requireBasicAuth(realm);
			return false;
		}
	}
}