#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

#include <maycloud/httpmessage.h>

class HttpRequest: public HttpMessage
{
protected:
	bool got_headers;
	bool got_body;
	std::string raw_headers;

public:
	HttpRequest();
	~HttpRequest();
	void parseHeaders();
	void feed(const std::string &data);
	std::string path();
	bool ready();
};

#endif
