#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <maycloud/httpmessage.h>
#include <maycloud/httpconnection.h>

class HttpMessage;
class HttpConnection;

class HttpResponse: public HttpMessage
{
protected:
	unsigned short int _status;

public:
	HttpResponse(HttpConnection *connection);
	~HttpResponse();

	void setStatus(int status);
	void setContentType(const std::string &content_type);
	void setBody(const std::string &body);

	std::string toString();

	std::string textByCode(int status);
};

#endif
