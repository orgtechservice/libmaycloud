#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <maycloud/httpmessage.h>
#include <maycloud/httpconnection.h>

class HttpMessage;
class HttpConnection;

class HttpResponse: public HttpMessage
{
protected:
	int _status;
	std::map<int, std::string> status_map;

public:
	HttpResponse(HttpConnection *connection);
	~HttpResponse();

	void setStatus(int code);
	void setContentType(const std::string &content_type);
	void setBody(const std::string &body);

	std::string toString();

	std::string statusText();
	std::string statusText(int code);
};

#endif
