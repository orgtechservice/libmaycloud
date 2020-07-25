#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

#include <maycloud/httpresponse.h>
#include <maycloud/httpmessage.h>

class HttpRequest: public HttpMessage
{
protected:
	bool got_headers;
	bool got_body;
	bool has_auth_info;
	std::string _raw_headers;
	std::string _raw_body;
	std::string _method;
	std::string _path;
	std::string _host;
	std::string _username;
	std::string _password;
	int _error;
	std::map<std::string, std::string> _cookies;
	std::map<std::string, std::string> _GET;
	std::map<std::string, std::string> _POST;

public:
	HttpRequest(HttpConnection *connection);
	~HttpRequest();
	void parseHeaders();
	void parseHeader(const std::string &name, const std::string &value);
	void parseRequestPath(const std::string &path);
	void parseBody();
	void feed(const std::string &data);
	std::string path();
	std::string method();
	std::string host();
	std::string get(const std::string &variable, const std::string &default_value = "");
	std::map<std::string, std::string> get();
	bool getVariableExists(const std::string &variable);
	bool ready();
	int error();
	bool hasAuthInfo();
	std::string username();
	std::string password();
	void handleBasicAuth(const std::string &base64_auth);
	bool authenticateUser(HttpResponse *response, const std::string &username, const std::string &password, const std::string &realm);
};

#endif
