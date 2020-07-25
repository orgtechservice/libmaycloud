
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
		size_t pos = data.find("\r\n\r\n");
		if(pos == std::string::npos) {
			_raw_headers += data;
		} else {
			std::string last_part = data.substr(0, pos);
			_raw_headers += last_part;
			got_headers = true;
			parseHeaders();

			std::string leftovers = data.substr(pos + 4);
			_raw_body += leftovers;
			if(_raw_body.length() >= _content_length) {
				got_body = true;
				parseBody();
			} else {
				//std::cout << "WTF, leftovers: <" << leftovers << ">, CL: " << _content_length << std::endl;
			}
		}
	} else {
		if(!got_body) {
			_raw_body += data;
			if(!hasLength()) {
				_error = 411;
			}
			if(_raw_body.length() >= _content_length) {
				got_body = true;
			}
		} else {
			parseBody();
		}
	}
}

void HttpRequest::parseHeaders() {
	//std::cout << "[HttpRequest::parseHeaders]" << std::endl;
	std::vector<std::string> supported_methods {"GET", "POST"};
	EasyVector lines = explode("\r\n", _raw_headers);
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
		parseRequestPath(parts[1]);
		//_path = parts[1];
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
	if(name == "Content-Length") {
		_content_length = strtoul(value.c_str(), 0, 0);
		//std::cout << "Got Content-Length, value: " << std::to_string(_content_length) << std::endl;
	}
	if(name == "Content-Type") {
		_content_type = value;
	}
}

void HttpRequest::parseBody() {
	if(_raw_body.empty()) return;

	if(_content_type == "application/x-www-form-urlencoded") {
		parseQueryString(_raw_body, &_POST);
		return;
	}

	/*if() { TODO multipart/form-data
		return;
	}*/

	_error = 501;
}

void HttpRequest::parseQueryString(const std::string &query_string, std::map<std::string, std::string> *vars) {
	EasyVector parts;
	EasyVector subparts;
	parts = explode("&", query_string);
	for(int i = 0; i < parts.size(); i ++) {
		// TODO handle arrays
		subparts = explode("=", parts[i]);
		if(subparts.size() == 1) {
			(*vars)[subparts[0]] = "";
			continue;
		}
		if(subparts.size() == 2) {
			(*vars)[subparts[0]] = urldecode(subparts[1]);
			continue;
		}
		_error = 400;
	}
}

void HttpRequest::parseRequestPath(const std::string &path) {
	size_t position = path.find('?');
	if(position == std::string::npos) {
		_path = path;
	} else {

		_path = path.substr(0, position);
		parseQueryString(path.substr(position + 1), &_GET);
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

std::string HttpRequest::get(const std::string &variable, const std::string &default_value) {
	auto it = _GET.find(variable);
	if(it == _GET.end()) {
		return default_value;
	} else {
		return it->second;
	}
}

std::map<std::string, std::string> HttpRequest::get() {
	return _GET;
}

std::string HttpRequest::post(const std::string &variable, const std::string &default_value) {
	auto it = _POST.find(variable);
	if(it == _POST.end()) {
		return default_value;
	} else {
		return it->second;
	}
}

std::map<std::string, std::string> HttpRequest::post() {
	return _POST;
}

bool HttpRequest::getVariableExists(const std::string &variable) {
	return (_GET.find(variable) != _GET.end());
}

bool HttpRequest::postVariableExists(const std::string &variable) {
	return (_POST.find(variable) != _POST.end());
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
