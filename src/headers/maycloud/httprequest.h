#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

#include <maycloud/httpmessage.h>

class HttpConnection;

class HttpRequest: public HttpMessage
{
protected:
	bool got_headers;
	bool got_body;
	std::string raw_headers;
	std::string _method;
	std::string _path;
	int _error;

public:
	HttpRequest(HttpConnection *connection);
	~HttpRequest();
	void parseHeaders();
	void feed(const std::string &data);
	std::string path();
	std::string method();
	bool ready();
	int error();
};

#endif
