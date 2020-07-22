#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <maycloud/httpmessage.h>

class HttpResponse: public HttpMessage
{
public:
	HttpResponse();
	~HttpResponse();

	void setStatus(unsigned short int status);
	void setContentType(const std::string &content_type);
	void setBody(const std::string &body);

	std::string toString();
};

#endif
