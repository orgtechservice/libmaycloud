#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>

#include <maycloud/httpmessage.h>

class HttpRequest: public HttpMessage
{
public:
	HttpRequest();
	~HttpRequest();
	void feed(const std::string &data);
};

#endif
