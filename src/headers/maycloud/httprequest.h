#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

#include <maycloud/httpresponse.h>
#include <maycloud/httpmessage.h>
#include <maycloud/randomstring.h>

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
	std::map<uint8_t, std::string> _route_params;

	bool parseMPPart(const std::string &data, const std::string &newline);
	bool parseMPPartBody(const std::map<std::string, std::string> &part_headers, const std::string &part_body);
	std::map<std::string, std::string> parseHeaderExtraLine(const std::string &extra);
	void parseHeader(const std::string &name, const std::string &value);
	void parseRequestPath(const std::string &path);
	void parseQueryString(const std::string &query_string, std::map<std::string, std::string> *vars);
	void parseMultipartFormData(const std::string &data, const std::string &boundary);
	void parseHeaders();
	void parseBody();
	void handleCookies(const std::string &cookies);

public:
	HttpRequest(HttpConnection *connection);
	~HttpRequest();
	void feed(const std::string &data);
	std::string path();
	std::string method();
	std::string host();
	std::string get(const std::string &variable, const std::string &default_value = "");
	std::map<std::string, std::string> get();
	std::string post(const std::string &variable, const std::string &default_value = "");
	std::map<std::string, std::string> post();
	bool getVariableExists(const std::string &variable);
	bool postVariableExists(const std::string &variable);
	bool ready();
	int error();
	bool hasAuthInfo();
	std::string username();
	std::string password();
	void handleBasicAuth(const std::string &base64_auth);
	bool authenticateUser(HttpResponse *response, const std::string &username, const std::string &password, const std::string &realm);
	void addRouteParam(const uint8_t &index, const std::string &value);
	std::string getRouteParam(const uint8_t &index, const std::string &default_value = "");
	void addRouteParams(const std::map<uint8_t, std::string> &params);
	std::map<uint8_t, std::string> getRouteParams();
	void initSession(HttpResponse *response, const std::string &cookie_name = "");
	bool hasCookie(const std::string &cookie_name);
	std::string cookie(const std::string &cookie_name, const std::string &default_value = "");
};

#endif
